#include "Controller.h"
#include "message_m.h"

#include <algorithm>
#include <cmath>
#include <cstring>   // for strcmp

namespace omnetpp {

Define_Module(Controller);

// -------------------------
// Helper: check freshness of a timestamp (so we dont compute based on old data)
// -------------------------
static bool fresh(simtime_t last, simtime_t maxAge) {
    return (last >= SIMTIME_ZERO) && ((simTime() - last) <= maxAge);
}

// -------------------------
// initialize()
// -------------------------
void Controller::initialize()
{
    // ---- read parameters (from NED/ini) ----
    numIntersections = par("numIntersections");
    numApproaches    = par("numApproaches");
    method           = par("method").stdstringValue();
    fcfsOwner = -1;

    TDthreshold = par("TDthreshold").doubleValue();
    Dthreshold  = par("Dthreshold").doubleValue(); // meters
    tClear      = par("tClear");                   // seconds

    tickPeriod  = par("tickPeriod");               // seconds

    // ---- allocate queue table: q[intersection][approach] ----
    q.assign(numIntersections, std::vector<QueueState>(numApproaches));

    // ---- initialize session ----
    session.active = false;
    session.evId = -1;
    session.intersectionId = -1;
    session.approach = -1;
    session.severity = 3;
    session.startedAt = SIMTIME_ZERO;
    session.lastCommandAt = SIMTIME_ZERO;
    session.inClear = false;
    session.pendingEvId = -1;

    // ---- periodic decision tick ----
    tick = new cMessage("tick");
    scheduleAt(simTime() + tickPeriod, tick);
}

// -------------------------
// haveFreshQueues(): do we have recent queue data for that intersection?
// -------------------------
bool Controller::haveFreshQueues(int interId) const
{
    if (interId < 0 || interId >= numIntersections) return false;

    // QueueReports are typically sent every ~1s; accept recent ones
    simtime_t maxAge = 2.0; // seconds

    for (int a = 0; a < numApproaches; a++) {
        if (fresh(q[interId][a].lastUpdate, maxAge)) return true;
    }
    return false;
}

// -------------------------
// computeDD_dynamic(): returns ET (seconds) to match your table notation
// ET = (C*L + (C-1)*MG)/S + r
// DD = ET * SEV, computed where needed using EV speed
// -------------------------
double Controller::computeDD_dynamic(int targetInter, int approach) const
{
    // Parameters from your fixed table (set in ini)
    double L  = par("L").doubleValue();            // 4.5 m
    double MG = par("MG").doubleValue();           // 2.5 m
    double S  = par("regularSpeed").doubleValue(); // m/s
    double r  = par("r").doubleValue();            // s

    int C = q[targetInter][approach].C;

    // If no queue, ET is basically just switching delay r
    if (C <= 0) return r;

    double ET = ((C * L) + ((C - 1) * MG)) / std::max(0.1, S) + r;
    return ET;
}

// -------------------------
// pickWinnerEvId(): multi-EV arbitration
// Candidate EV must satisfy: DEMV <= DD (trigger condition)
// Rule order:
//   1) higher priority (smaller severity)
//   2) smaller EAT (arrives sooner)
//   3) if equal EAT: closer to accident position (smaller distToAP)
//   4) if still tie: FCFS (earlier tSent; else earlier lastSeen)
// -------------------------
int Controller::pickWinnerEvId() const
{
    int bestId = -1;

    for (const auto& kv : evs) {
        const EVState& ev = kv.second;

        // ignore stale EV updates
        if (!fresh(ev.lastSeen, 1.0)) continue;

        // basic bounds
        if (ev.targetInter < 0 || ev.targetInter >= numIntersections) continue;
        if (ev.approach < 0 || ev.approach >= numApproaches) continue;

        // need queues to compute ET/DD
        if (!haveFreshQueues(ev.targetInter)) continue;

        // Trigger: DEMV <= DD
        double ET_sec = computeDD_dynamic(ev.targetInter, ev.approach); // seconds
        double DD_m = ET_sec * std::max(0.1, std::fabs(ev.speed));

        if (ev.DEMV > DD_m) continue; //not within preemption distance

        if (bestId == -1) {
            bestId = ev.evId;
            continue;
        }

        const EVState& best = evs.at(bestId);

        // 1) severity (priority)
        if (ev.severity < best.severity) {
            bestId = ev.evId;
            continue;
        }
        if (ev.severity > best.severity) continue;

        // 2) EAT (smaller is earlier)
        double evEAT = ev.eat();
        double bestEAT = best.eat();

        if (evEAT < bestEAT) {
            bestId = ev.evId;
            continue;
        }
        if (evEAT > bestEAT) continue;

        // 3) equal EAT -> closer to accident position (smaller distToAP)
        if (ev.distToAP < best.distToAP) {
            bestId = ev.evId;
            continue;
        }
        if (ev.distToAP > best.distToAP) continue;

        // 4) still tie -> FCFS (earlier request)
        simtime_t evReq   = (ev.tSent >= SIMTIME_ZERO) ? ev.tSent : ev.lastSeen;
        simtime_t bestReq = (best.tSent >= SIMTIME_ZERO) ? best.tSent : best.lastSeen;

        if (evReq < bestReq) {
            bestId = ev.evId;
            continue;
        }
    }

    return bestId;
}

// -------------------------
// sendCmd(): create and send a TlCommand to an intersection
// action: "PREEMPT", "CLEAR", "RECOVERY", "NORMAL"
// durationSec used for CLEAR and RECOVERY (Intersection uses it)
// -------------------------
void Controller::sendCmd(int interId, int approach, const char *action, double durationSec)
{
    TlCommand *cmd = new TlCommand("TlCommand");
    cmd->setIntersectionId(interId);
    cmd->setApproach(approach);
    cmd->setAction(action);
    cmd->setDuration(durationSec);

    // Assumes vector gate: toIntersection[numIntersections]
    send(cmd, "toIntersection", interId);
}

// -------------------------
// startSession(): begin preemption for a winner EV
// -------------------------
void Controller::startSession(const EVState& ev)
{
    session.active = true;
    session.evId = ev.evId;
    session.intersectionId = ev.targetInter;
    session.approach = ev.approach;
    session.severity = ev.severity;
    session.startedAt = simTime();
    session.lastCommandAt = simTime();
    session.inClear = false;
    session.pendingEvId = -1;

    sendCmd(session.intersectionId, session.approach, "PREEMPT", 0.0);
}

// -------------------------
// switchSessionWithClear(): HARD override with safety buffer
// CLEAR for tClear, then PREEMPT for pending EV
// -------------------------
void Controller::switchSessionWithClear(const EVState& newWinner)
{
    session.inClear = true;
    session.pendingEvId = newWinner.evId;
    session.lastCommandAt = simTime();

    // All-red at the currently controlled intersection
    sendCmd(session.intersectionId, -1, "CLEAR", tClear.dbl()); //after tClear,  switch to the new EV
}

// -------------------------
// endSessionToNormal(): stop current session, apply recovery, return to normal
// Recovery may include previous TL if distance is below Dthreshold
// -------------------------
void Controller::endSessionToNormal()
{
    if (!session.active) return;

    double recoveryDuration = par("recoveryDuration").doubleValue(); // seconds
    double roadSegLen = par("roadSegmentLength").doubleValue();      // meters

    // Recovery at current intersection
    sendCmd(session.intersectionId, -1, "RECOVERY", recoveryDuration);

    // If consecutive TLs are closer than Dthreshold, also recover previous TL (part 3 of the sd-tlp alg)
    if (session.intersectionId > 0 && roadSegLen < Dthreshold) {
        sendCmd(session.intersectionId - 1, -1, "RECOVERY", recoveryDuration);
    }

    // Return to NORMAL
    sendCmd(session.intersectionId, -1, "NORMAL", 0.0);
    if (session.intersectionId > 0 && roadSegLen < Dthreshold) {
        sendCmd(session.intersectionId - 1, -1, "NORMAL", 0.0);
    }

    // clear session
    session.active = false;
    session.evId = -1;
    session.intersectionId = -1;
    session.approach = -1;
    session.severity = 3;
    session.inClear = false;
    session.pendingEvId = -1;
}

// -------------------------
// applyNoPreempt(): baseline
// -------------------------
void Controller::applyNoPreempt()
{
    if (session.active) endSessionToNormal();
}

// -------------------------
// applyFcfs(): baseline
// First EV that satisfies DEMV <= DD gets served; no override.
// Tie: earliest tSent (or lastSeen)
// -------------------------
/*void Controller::applyFcfs()
{
    int chosen = -1;
    simtime_t bestReq = SIMTIME_ZERO;

    // ---------------------------------------------------------
    // (A) FCFS WINNER SELECTION (among eligible EVs)
    // Eligible = fresh + has queues + DEMV <= DD
    // FCFS key = earliest tSent (else lastSeen)
    // ---------------------------------------------------------
    for (const auto& kv : evs) {
        const EVState& ev = kv.second;
        if (!fresh(ev.lastSeen, 1.0)) continue;

        if (ev.targetInter < 0 || ev.targetInter >= numIntersections) continue;
        if (ev.approach < 0 || ev.approach >= numApproaches) continue;
        if (!haveFreshQueues(ev.targetInter)) continue;

        // Trigger: DEMV <= DD
        double ET_sec = computeDD_dynamic(ev.targetInter, ev.approach);
        double DD_m = ET_sec * std::max(0.1, std::fabs(ev.speed));

        if (ev.DEMV > DD_m) continue;

        simtime_t req = (ev.tSent >= SIMTIME_ZERO) ? ev.tSent : ev.lastSeen;

        if (chosen == -1 || req < bestReq) {
            chosen = ev.evId;
            bestReq = req;
        }
    }

    // ---------------------------------------------------------
    // (B) If no eligible EV, end current session (if any)
    // ---------------------------------------------------------
    if (chosen == -1) {
        if (session.active) endSessionToNormal();
        return;
    }

    const EVState& winner = evs.at(chosen);

    // ---------------------------------------------------------
    // (C) Start/maintain session for the FCFS winner
    // IMPORTANT: no hard override in this baseline
    // ---------------------------------------------------------
    if (!session.active) {
        startSession(winner);
    } else {
        // No override: if another EV becomes "earlier" later, we ignore it
        if (session.evId != winner.evId) {
            // do nothing (baseline stays with the current session owner)
            // You can also choose to end session and restart, but that is NOT FCFS.
        } else {
            // Same EV moves intersection-by-intersection
            if (session.intersectionId != winner.targetInter) {
                endSessionToNormal();
                startSession(winner);
            }
        }
    }

    // ---------------------------------------------------------
    // (D) SD-TLP LOOKAHEAD (next two intersections) for FCFS winner
    // Only apply lookahead for the selected FCFS EV
    // ---------------------------------------------------------
    for (int k = 1; k <= 2; k++) {
        int nextInter = winner.targetInter + k;
        if (nextInter >= numIntersections) continue;
        if (!haveFreshQueues(nextInter)) continue;

        // SD-TLP Part 2 condition: preempt next TLs if congested
        if (q[nextInter][winner.approach].TD > TDthreshold) {
            sendCmd(nextInter, winner.approach, "PREEMPT", 0.0);
        }
    }
    return;

}
*/void Controller::applyFcfs()
{
    // =====================================================
    // (A) If FCFS owner already exists → keep serving it
    // =====================================================
    if (fcfsOwner != -1) {

        // Owner disappeared or stale -> release owner and go normal
        if (evs.find(fcfsOwner) == evs.end() ||
            !fresh(evs.at(fcfsOwner).lastSeen, 1.0)) {

            fcfsOwner = -1;
            if (session.active) endSessionToNormal();
            return;
        }

        const EVState& owner = evs.at(fcfsOwner);

        // Start/advance the session for the owner
        if (!session.active) {
            startSession(owner);
        }
        else if (session.evId == owner.evId &&
                 session.intersectionId != owner.targetInter) {
            endSessionToNormal();
            startSession(owner);
        }

        // SD-TLP lookahead (next two intersections) for FCFS owner only
        for (int k = 1; k <= 2; k++) {
            int nextInter = owner.targetInter + k;
            if (nextInter >= numIntersections) continue;
            if (!haveFreshQueues(nextInter)) continue;

            if (q[nextInter][owner.approach].TD > TDthreshold) {
                sendCmd(nextInter, owner.approach, "PREEMPT", 0.0);
            }
        }
        return;
    }

    // =====================================================
    // (B) No owner yet → pick FCFS winner among ELIGIBLE EVs
    // FCFS key = earliest time EV FIRST became eligible (entered DD)
    // FIX: stamp eligibleSince using ev.tSent (EV-side time) not controller tick time
    // FIX: explicit tie-break to avoid implicit "lower evId wins"
    // =====================================================
    int chosen = -1;
    simtime_t bestEligibleSince = SIMTIME_ZERO;

    for (auto& kv : evs) {
        EVState& ev = kv.second;

        // must be fresh
        if (!fresh(ev.lastSeen, 1.0)) continue;

        // bounds
        if (ev.targetInter < 0 || ev.targetInter >= numIntersections) continue;
        if (ev.approach < 0 || ev.approach >= numApproaches) continue;

        // need queues (for ET/DD)
        if (!haveFreshQueues(ev.targetInter)) continue;

        // Trigger: DEMV <= DD
        double ET_sec = computeDD_dynamic(ev.targetInter, ev.approach);
       double DD_m   = ET_sec * std::max(0.1, std::fabs(ev.speed));
       // double DD_m = par("fixedFcfsDD").doubleValue(); //renove
       int C = q[ev.targetInter][ev.approach].C;
       EV_INFO << "[ELIG] t=" << simTime()
               << " ev=" << ev.evId
               << " inter=" << ev.targetInter
               << " app=" << ev.approach
               << " C=" << C
               << " ET=" << ET_sec
               << " DD=" << DD_m
               << " DEMV=" << ev.DEMV
               << " eligible=" << (ev.DEMV <= DD_m)
               << "\n";

        if (ev.DEMV > DD_m) continue;   // not eligible yet

        // ---- FIX #1: store first eligibility time using EV timestamp ----
        if (!ev.hasEligibleSince) {
            ev.eligibleSince = (ev.tSent >= SIMTIME_ZERO) ? ev.tSent : simTime();
            ev.hasEligibleSince = true;
        }

        // ---- FIX #2: deterministic tie-break (choose smaller eligibleSince, then smaller evId) ----
        if (chosen == -1 ||
            ev.eligibleSince < bestEligibleSince ||
            (ev.eligibleSince == bestEligibleSince && ev.evId < chosen)) {
            chosen = ev.evId;
            bestEligibleSince = ev.eligibleSince;
        }
    }

    // No eligible EVs
    if (chosen == -1) {
        if (session.active) endSessionToNormal();
        return;
    }

    // Lock FCFS owner and start session
    fcfsOwner = chosen;
    startSession(evs.at(fcfsOwner));
}

// -------------------------
// applySdtlpSingle(): original SD-TLP single-EV behavior
// (if multiple EVs exist, we just choose the earliest EAT eligible candidate,
// but we DO NOT do hard override switching like the multi extension)
// -------------------------
void Controller::applySdtlpSingle()
{
    int candidate = -1;
    double bestEat = 1e18;

    for (const auto& kv : evs) {
        const EVState& ev = kv.second;
        if (!fresh(ev.lastSeen, 1.0)) continue;

        if (ev.targetInter < 0 || ev.targetInter >= numIntersections) continue;
        if (ev.approach < 0 || ev.approach >= numApproaches) continue;
        if (!haveFreshQueues(ev.targetInter)) continue;

        double ET_sec = computeDD_dynamic(ev.targetInter, ev.approach);
        double DD_m = ET_sec * std::max(0.1, std::fabs(ev.speed));

        if (ev.DEMV > DD_m) continue;

        if (candidate == -1 || ev.eat() < bestEat) {
            candidate = ev.evId;
            bestEat = ev.eat();
        }
    }

    if (candidate == -1) {
        if (session.active) endSessionToNormal();
        return;
    }

    const EVState& ev = evs.at(candidate);

    if (!session.active) {
        startSession(ev);
    } else {
        // same EV progresses to next intersection
        if (session.evId == ev.evId && session.intersectionId != ev.targetInter) {
            endSessionToNormal();
            startSession(ev);
        }
    }

    // Part 2 lookahead: next two intersections if congested on EV approach
    for (int k = 1; k <= 2; k++) {
        int nextInter = ev.targetInter + k;
        if (nextInter >= numIntersections) continue;
        if (!haveFreshQueues(nextInter)) continue;

        if (q[nextInter][ev.approach].TD > TDthreshold) {
            sendCmd(nextInter, ev.approach, "PREEMPT", 0.0);
        }
    }
}

// -------------------------
// applySdtlpMulti(): YOUR extended SD-TLP
// - winner = severity, then EAT, then distToAP, then FCFS
// - HARD override: if a different EV becomes winner (and is inside DD), switch immediately
// - Do NOT allow a losing EV to keep lookahead reservations.
// -------------------------
void Controller::applySdtlpMulti()
{
    int winnerId = pickWinnerEvId();

    if (winnerId == -1) {
        if (session.active) endSessionToNormal();
        return;
    }

    const EVState& winner = evs.at(winnerId);

    // If currently in CLEAR, wait until it expires then start pending EV
    if (session.active && session.inClear) {
        if ((simTime() - session.lastCommandAt) >= tClear) {
            session.inClear = false;

            int pid = session.pendingEvId;
            session.pendingEvId = -1;

            if (pid != -1 && evs.find(pid) != evs.end()) {
                startSession(evs.at(pid));
            }
        }
        return;
    }

    // If no session, start immediately
    if (!session.active) {
        startSession(winner);
    }
    else {
        // If a different EV is now the winner, HARD override:
        if (session.evId != winner.evId) {

            // Cancel previous EV's lookahead reservations (so it cannot keep next TLs)
            for (int k = 1; k <= 2; k++) {
                int nextInter = session.intersectionId + k;
                if (nextInter >= numIntersections) continue;
                sendCmd(nextInter, -1, "NORMAL", 0.0);
            }

            switchSessionWithClear(winner);
            return;
        }

        // Same EV continues: if it moved to another intersection, move session
        if (session.intersectionId != winner.targetInter) {
            endSessionToNormal();
            startSession(winner);
        }
    }

    // Lookahead is applied ONLY for the current global winner EV
    for (int k = 1; k <= 2; k++) {
        int nextInter = winner.targetInter + k;
        if (nextInter >= numIntersections) continue;
        if (!haveFreshQueues(nextInter)) continue;

        if (q[nextInter][winner.approach].TD > TDthreshold) {
            sendCmd(nextInter, winner.approach, "PREEMPT", 0.0);
        }
    }
}

// -------------------------
// handleMessage()
// -------------------------
void Controller::handleMessage(cMessage *msg)
{
    // ---- QueueReport ----
    if (strcmp(msg->getName(), "QueueReport") == 0) {
        QueueReport *qr = check_and_cast<QueueReport*>(msg);

        int inter = qr->getIntersectionId();
        int app   = qr->getApproach();

        if (inter >= 0 && inter < numIntersections &&
            app >= 0 && app < numApproaches) {
            q[inter][app].C = qr->getC();
            q[inter][app].TD = qr->getTD();
            q[inter][app].lastUpdate = simTime();
        }

        delete qr;
        return;
    }

    // ---- EvUpdate ----
    if (strcmp(msg->getName(), "EvUpdate") == 0) {
        EvUpdate *eu = check_and_cast<EvUpdate*>(msg);
        int id = eu->getEvId(); //new

        // Create once if new (so we do NOT erase FCFS memory fields) new
           if (evs.find(id) == evs.end()) {
               EVState tmp;
               tmp.evId = id;
               evs[id] = tmp;
           }//new

        //EVState ev;
           EVState& ev = evs[id]; //new
        ev.evId = eu->getEvId();
        ev.severity = eu->getSeverity();
        ev.targetInter = eu->getTargetInter();
        ev.approach = eu->getApproach();
        ev.DEMV = eu->getDEMV();
        ev.speed = eu->getSpeed();
        ev.tSent = eu->getTSent();
        ev.distToAP = eu->getDistToAP();
        ev.lastSeen = simTime();

       // evs[ev.evId] = ev;

        delete eu;
        return;
    }

    // ---- periodic tick ----
    if (msg == tick) {

        if (method == "NO_PREEMPT") {
            applyNoPreempt();
        }
        else if (method == "FCFS") {
            applyFcfs();
        }
        else if (method == "SDTLP") {
            applySdtlpSingle();
        }
        else if (method == "SDTLP_MULTI") {
            applySdtlpMulti();
        }
        else {
            applyNoPreempt();
        }

        scheduleAt(simTime() + tickPeriod, tick);
        return;
    }

    delete msg;
}

void Controller::finish()
{
    cancelAndDelete(tick);
}

} // namespace omnetpp
