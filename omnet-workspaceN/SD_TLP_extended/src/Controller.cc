/*#include "Controller.h"
#include "message_m.h"

#include <algorithm>
#include <cmath>
#include <cstring>   // for strcmp

namespace omnetpp {

Define_Module(Controller);

 //check freshness of a timestamp (so we dont compute based on old data)
 static bool fresh(simtime_t last, simtime_t maxAge) {//maxage is usually 1 or 2 seconds
    return (last >= SIMTIME_ZERO) && ((simTime() - last) <= maxAge);
}


void Controller::initialize()
{
    // read parameters (from NED/ini)
    numIntersections = par("numIntersections");
    numApproaches    = par("numApproaches");
    method           = par("method").stdstringValue();
    fcfsOwner = -1;
    TDthreshold = par("TDthreshold").doubleValue();
    Dthreshold  = par("Dthreshold").doubleValue();
    tClear      = par("tClear");
    tickPeriod  = par("tickPeriod");

    // allocate queue table: q[intersection][approach]
    q.assign(numIntersections, std::vector<QueueState>(numApproaches));

    //   initialize session
    session.active = false;
    session.evId = -1;
    session.intersectionId = -1;
    session.approach = -1;
    session.severity = 3;
    session.startedAt = SIMTIME_ZERO;
    session.lastCommandAt = SIMTIME_ZERO;
    session.inClear = false;
    session.pendingEvId = -1;

     tick = new cMessage("tick");
    scheduleAt(simTime() + tickPeriod, tick);
}

 // haveFreshQueues(): do we have recent queue data for that intersection?
 bool Controller::haveFreshQueues(int interId) const
{
    if (interId < 0 || interId >= numIntersections) return false;

    // QueueReports are typically sent every 1s--> accept recent ones
    simtime_t maxAge = 2.0;

    for (int a = 0; a < numApproaches; a++) {
        if (fresh(q[interId][a].lastUpdate, maxAge)) return true;
    }
    return false;
}

// computeDD_dynamic(): returns ET (seconds)
// ET = (C*L + (C-1)*MG)/S + r
// DD = ET * SEV
 double Controller::computeDD_dynamic(int targetInter, int approach) const
{

    double L  = par("L").doubleValue();            // 4.5 m
    double MG = par("MG").doubleValue();           // 2.5 m
    double S  = par("regularSpeed").doubleValue();
    double r  = par("r").doubleValue();

    int C = q[targetInter][approach].C; //from the QueueReport messages

    // If no queue, ET is basically just switching delay r
    if (C <= 0) return r;

    double ET = ((C * L) + ((C - 1) * MG)) / std::max(0.1, S) + r;
    return ET;
}

 // pickWinnerEvId(): multi-EV selection
// Candidate EV must satisfy: DEMV <= DD
// Rule order:
//   1) higher priority (smaller severity)
//   2) smaller EAT (arrives sooner)
//   3) if equal EAT: closer to accident position (smaller distToAP)
//   4) if still tie: FCFS (earlier tSent; else earlier lastSeen)
 int Controller::pickWinnerEvId() const
{
    int bestId = -1;

    for (const auto& kv : evs) { //Loop over all EVs
        const EVState& ev = kv.second;

        // ignore old EV updates
        if (!fresh(ev.lastSeen, 1.0)) continue;

        // basic bounds (checking incase)
        if (ev.targetInter < 0 || ev.targetInter >= numIntersections) continue;
        if (ev.approach < 0 || ev.approach >= numApproaches) continue;

        // need queues to compute ET/DD
        if (!haveFreshQueues(ev.targetInter)) continue;

        // DEMV <= DD
        double ET_sec = computeDD_dynamic(ev.targetInter, ev.approach);
        double DD_m = ET_sec * std::max(0.1, std::fabs(ev.speed));

        if (ev.DEMV > DD_m) continue; //not within preemption distance

        if (bestId == -1) {//if no best, then its the best
            bestId = ev.evId;
            continue;
        }

        const EVState& best = evs.at(bestId);
       //if best exists --> it will compare
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

 // sendCmd(): create and send a TlCommand to an intersection
// action: "PREEMPT", "CLEAR", "RECOVERY", "NORMAL"
// durationSec used for CLEAR and RECOVERY (Intersection uses it)
 void Controller::sendCmd(int interId, int approach, const char *action, double durationSec)
{
    TlCommand *cmd = new TlCommand("TlCommand");
    cmd->setIntersectionId(interId);
    cmd->setApproach(approach);
    cmd->setAction(action);
    cmd->setDuration(durationSec);//tells the intersection how long the command should last

    // Assumes vector gate: toIntersection[numIntersections]
    send(cmd, "toIntersection", interId);
}

 // startSession(): begin preemption for a winner EV
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

 // switchSessionWithClear(): HARD override with safety buffer
// CLEAR for tClear, then PREEMPT for pending EV
 void Controller::switchSessionWithClear(const EVState& newWinner)
{
    session.inClear = true;
    session.pendingEvId = newWinner.evId;
    session.lastCommandAt = simTime();

    // All-red at the currently controlled intersection
    sendCmd(session.intersectionId, -1, "CLEAR", tClear.dbl()); //after tClear,  switch to the new EV
}

 // endSessionToNormal(): stop current session, apply recovery, return to normal
// Recovery may include previous TL if distance is below Dthreshold
 //part 3 of sdtlp
void Controller::endSessionToNormal()
{
    if (!session.active) return;

    double recoveryDuration = par("recoveryDuration").doubleValue();  //5 seconds
    double roadSegLen = par("roadSegmentLength").doubleValue();

    // Recovery at current intersection (forces preempted approach red for recoveryDuration)
    sendCmd(session.intersectionId, -1, "RECOVERY", recoveryDuration);

    // If consecutive TLs are closer than Dthreshold, also recover previous TL
    if (session.intersectionId > 0 && roadSegLen < Dthreshold) {
        sendCmd(session.intersectionId - 1, -1, "RECOVERY", recoveryDuration);
    }

     // Intersection will automatically end recovery after recoveryDuration
    // clear session
    session.active = false;
    session.evId = -1;
    session.intersectionId = -1;
    session.approach = -1;
    session.severity = 3;
    session.inClear = false;
    session.pendingEvId = -1;
}
 // applyNoPreempt(): baseline
 void Controller::applyNoPreempt()
{
    if (session.active) endSessionToNormal();
}

 // applyFcfs(): baseline
// First EV that satisfies DEMV <= DD gets served; no override.
// Tie: earliest tSent (or lastSeen)

void Controller::applyFcfs()
{
     //  If currently in CLEAR (all-red), wait then start pending EV
     if (session.inClear) {
        if ((simTime() - session.lastCommandAt) >= tClear) {// if true--> clear (all-red) time finish
            session.inClear = false;

            int pid = session.pendingEvId;//Get the EV that was waiting to start
            session.pendingEvId = -1;

            if (pid != -1 && evs.find(pid) != evs.end()) {
                startSession(evs.at(pid));
            } else {
                // Nobody to start; clear session
                session.active = false;
                session.evId = -1;
                session.intersectionId = -1;
                session.approach = -1;
            }
        }
        return;
    }

     //   eligibility check = fresh + valid target + have queues + DEMV <= DD
     auto isEligible = [&](EVState& ev) -> bool {
        if (!fresh(ev.lastSeen, 1.0)) return false;
        if (ev.targetInter < 0 || ev.targetInter >= numIntersections) return false;
        if (ev.approach < 0 || ev.approach >= numApproaches) return false;
        if (!haveFreshQueues(ev.targetInter)) return false;

        double ET_sec = computeDD_dynamic(ev.targetInter, ev.approach);
        double DD_m   = ET_sec * std::max(0.1, std::fabs(ev.speed));
        return (ev.DEMV <= DD_m);
    };

     // We only want CLEAR when a HANDOVER happens.
    // Track if the owner finished/ended this tick.
    bool handoverOccurredThisTick = false;
    int  clearInterAfterFinish    = -1;   // the intersection just served (the one we will force all-red)

     // If FCFS owner exists -> keep serving it (NO override)
    // If owner finished current intersection -> end session, release owner,
    // mark handover, then pick next winner
    // Also: if owner disappears or becomes old, treat it as finished
    // and enforce CLEAR before the next EV for fair comparison.
     if (fcfsOwner != -1) {

        //  Owner disappeared or old -> treat as FINISH + HANDOVER boundary
        if (evs.find(fcfsOwner) == evs.end() ||
            !fresh(evs.at(fcfsOwner).lastSeen, 1.0)) {

            int finishedId = fcfsOwner;

            if (session.active) {
                clearInterAfterFinish = session.intersectionId; // where we were serving
                endSessionToNormal();                           // SD-TLP Part 3 (RECOVERY)
                handoverOccurredThisTick = true;                // enforce CLEAR before next EV
            }

            fcfsOwner = -1;

            // fairness: let this EV re-compete later (if it still exists)
            if (evs.find(finishedId) != evs.end()) {
                evs[finishedId].hasEligibleSince = false;
            }

         }
        else {
            const EVState& owner = evs.at(fcfsOwner);

            // Start session if not active yet
            if (!session.active) {
                startSession(owner);

                // Lookahead (SD-TLP Step 2) for owner only
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

            // Owner finished intersection: targetInter changed
            if (session.evId == owner.evId &&
                session.intersectionId != owner.targetInter) {

                clearInterAfterFinish = session.intersectionId; // intersection just served
                int finishedId = fcfsOwner;

                endSessionToNormal();           // SD-TLP Part 3 (RECOVERY)
                fcfsOwner = -1;                 // release ownership so someone else can be chosen

                // fairness: let this EV re-compete at the next intersection
                if (evs.find(finishedId) != evs.end()) {
                    evs[finishedId].hasEligibleSince = false;
                }

                handoverOccurredThisTick = true; // enforce CLEAR before next EV starts
             }
            else {
                // Still serving same owner at same intersection
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
        }
    }

     //   No owner (or released) -> pick FCFS winner among ELIGIBLE EVs
    // winner = smallest eligibleSince; tie -> smaller evId
     int chosen = -1;
    simtime_t bestEligibleSince = SIMTIME_ZERO;

    for (auto& kv : evs) {//loop over all EVs
        EVState& ev = kv.second;
        if (!isEligible(ev)) continue;

         if (!ev.hasEligibleSince) {//record the time the the first time an EV becomes eligible
            ev.eligibleSince = (ev.tSent >= SIMTIME_ZERO) ? ev.tSent : simTime();
            ev.hasEligibleSince = true;
        }

        if (chosen == -1 ||
            ev.eligibleSince < bestEligibleSince ||
            (ev.eligibleSince == bestEligibleSince && ev.evId < chosen)) {
            chosen = ev.evId;
            bestEligibleSince = ev.eligibleSince;
        }
    }

    // No eligible EVs -> end any active session and return
    if (chosen == -1) {
        if (session.active) endSessionToNormal();
        return;
    }

    fcfsOwner = chosen;

     //  Start next EV:
    //  If handover occurred -> do CLEAR first then start pending EV.
    // Otherwise -> start immediately.

    if (handoverOccurredThisTick) {//this means the previous EV just finished/disappeared, so do safe handover.
        session.active = true;
        session.inClear = true;
        session.pendingEvId = fcfsOwner;
        session.lastCommandAt = simTime();

        // Clear the intersection that was just served (fair handover)
        int interToClear = (clearInterAfterFinish >= 0)
                         ? clearInterAfterFinish
                         : evs.at(fcfsOwner).targetInter; // fallback (should rarely happen)

        sendCmd(interToClear, -1, "CLEAR", tClear.dbl());
        return; // start pending EV after CLEAR expires
    }

    // No handover: start immediately
    startSession(evs.at(fcfsOwner));

    // Lookahead (SD-TLP Step 2) for the new owner
    const EVState& newOwner = evs.at(fcfsOwner);
    for (int k = 1; k <= 2; k++) {
        int nextInter = newOwner.targetInter + k;
        if (nextInter >= numIntersections) continue;
        if (!haveFreshQueues(nextInter)) continue;

        if (q[nextInter][newOwner.approach].TD > TDthreshold) {
            sendCmd(nextInter, newOwner.approach, "PREEMPT", 0.0);
        }
    }
}
 // applySdtlpSingle(): original SD-TLP single-EV behavior
  void Controller::applySdtlpSingle()
{
    int candidate = -1;

    for (const auto& kv : evs) {
        const EVState& ev = kv.second;

        if (!fresh(ev.lastSeen, 1.0)) continue;
        if (ev.targetInter < 0 || ev.targetInter >= numIntersections) continue;
        if (ev.approach < 0 || ev.approach >= numApproaches) continue;
        if (!haveFreshQueues(ev.targetInter)) continue;

        double ET_sec = computeDD_dynamic(ev.targetInter, ev.approach);
        double DD_m = ET_sec * std::max(0.1, std::fabs(ev.speed));
        // Only eligible if EV is within preemption distance
        if (ev.DEMV > DD_m) continue;
            candidate = ev.evId;
            break;   // single-EV mode: first eligible EV is enough
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

 // applySdtlpMulti(): extended SD-TLP
//  winner = severity, then EAT, then distToAP, then FCFS
// HARD override: if a different EV becomes winner (and is inside DD), switch immediately
//  Do NOT allow a losing EV to keep lookahead reservations.
 void Controller::applySdtlpMulti()
{
    int winnerId = pickWinnerEvId();

    if (winnerId == -1) {//no winner
        if (session.active) endSessionToNormal();//recovery--> part 3 of sd-tlp
        return;
    }

    const EVState& winner = evs.at(winnerId);//get state about the winner

    // If currently in CLEAR, wait until it expires then start pending EV
    if (session.active && session.inClear) {
        if ((simTime() - session.lastCommandAt) >= tClear) {//true then all-red time done
            session.inClear = false;

            int pid = session.pendingEvId;
            session.pendingEvId = -1;

            if (pid != -1 && evs.find(pid) != evs.end()) {
                startSession(evs.at(pid));
            }
        }
        return;
    }
    EV_INFO << "[MULTI_START] t=" << simTime()
            << " winner=" << winner.evId
            << " inter=" << winner.targetInter
            << " app=" << winner.approach
            << " DEMV=" << winner.DEMV
            << " C=" << q[winner.targetInter][winner.approach].C
            << "\n";
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

    // Lookahead is applied ONLY for the current global winner EV (step 2 of sdtlp)
    for (int k = 1; k <= 2; k++) {
        int nextInter = winner.targetInter + k;
        if (nextInter >= numIntersections) continue;
        if (!haveFreshQueues(nextInter)) continue;

        if (q[nextInter][winner.approach].TD > TDthreshold) {
            sendCmd(nextInter, winner.approach, "PREEMPT", 0.0);
        }
    }
}

 // handleMessage()
 void Controller::handleMessage(cMessage *msg)
{
    // QueueReport
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

    //  EvUpdate
    if (strcmp(msg->getName(), "EvUpdate") == 0) {
        EvUpdate *eu = check_and_cast<EvUpdate*>(msg);
        int id = eu->getEvId(); //new

        // Create once if new (so we do NOT erase FCFS memory fields) new
           if (evs.find(id) == evs.end()) {
               EVState tmp;
               tmp.evId = id;
               evs[id] = tmp;
           }

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
*/
#include "Controller.h"
#include "message_m.h"

#include <algorithm>
#include <cmath>
#include <cstring>   // for strcmp

namespace omnetpp {

Define_Module(Controller);

 //check freshness of a timestamp (so we dont compute based on old data)
 static bool fresh(simtime_t last, simtime_t maxAge) {//maxage is usually 1 or 2 seconds
    return (last >= SIMTIME_ZERO) && ((simTime() - last) <= maxAge);
}


void Controller::initialize()
{
    // read parameters (from NED/ini)
    numIntersections = par("numIntersections");
    numApproaches    = par("numApproaches");
    method           = par("method").stdstringValue();
    fcfsOwner = -1;
    TDthreshold = par("TDthreshold").doubleValue();
    Dthreshold  = par("Dthreshold").doubleValue();
    tClear      = par("tClear");
    tickPeriod  = par("tickPeriod");

    // allocate queue table: q[intersection][approach]
    q.assign(numIntersections, std::vector<QueueState>(numApproaches));

    //   initialize session
    session.active = false;
    session.evId = -1;
    session.intersectionId = -1;
    session.approach = -1;
    session.severity = 3;
    session.startedAt = SIMTIME_ZERO;
    session.lastCommandAt = SIMTIME_ZERO;
    session.inClear = false;
    session.pendingEvId = -1;

     tick = new cMessage("tick");
    scheduleAt(simTime() + tickPeriod, tick);
}

 // haveFreshQueues(): do we have recent queue data for that intersection?
 bool Controller::haveFreshQueues(int interId) const
{
    if (interId < 0 || interId >= numIntersections) return false;

    // QueueReports are typically sent every 1s--> accept recent ones
    simtime_t maxAge = 2.0;

    for (int a = 0; a < numApproaches; a++) {
        if (fresh(q[interId][a].lastUpdate, maxAge)) return true;
    }
    return false;
}

// computeDD_dynamic(): returns ET (seconds)
// ET = (C*L + (C-1)*MG)/S + r
// DD = ET * SEV
 double Controller::computeDD_dynamic(int targetInter, int approach) const
{

    double L  = par("L").doubleValue();            // 4.5 m
    double MG = par("MG").doubleValue();           // 2.5 m
    double S  = par("regularSpeed").doubleValue();
    double r  = par("r").doubleValue();

    int C = q[targetInter][approach].C; //from the QueueReport messages

    // If no queue, ET is basically just switching delay r
    if (C <= 0) return r;

    double ET = ((C * L) + ((C - 1) * MG)) / std::max(0.1, S) + r;
    return ET;
}

 // pickWinnerEvId(): multi-EV selection
// Candidate EV must satisfy: DEMV <= DD
// Rule order:
//   1) higher priority (smaller severity)
//   2) smaller EAT (arrives sooner)
//   3) if equal EAT: closer to accident position (smaller distToAP)
//   4) if still tie: FCFS (earlier tSent; else earlier lastSeen)
 int Controller::pickWinnerEvId() const
{
    int bestId = -1;

    for (const auto& kv : evs) { //Loop over all EVs
        const EVState& ev = kv.second;

        // ignore old EV updates
        if (!fresh(ev.lastSeen, 1.0)) continue;

        // basic bounds (checking incase)
        if (ev.targetInter < 0 || ev.targetInter >= numIntersections) continue;
        if (ev.approach < 0 || ev.approach >= numApproaches) continue;

        // need queues to compute ET/DD
        if (!haveFreshQueues(ev.targetInter)) continue;

        // DEMV <= DD
        double ET_sec = computeDD_dynamic(ev.targetInter, ev.approach);
        double DD_m = ET_sec * std::max(0.1, std::fabs(ev.speed));

        if (ev.DEMV > DD_m) continue; //not within preemption distance

        if (bestId == -1) {//if no best, then its the best
            bestId = ev.evId;
            continue;
        }

        const EVState& best = evs.at(bestId);
       //if best exists --> it will compare
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

 // sendCmd(): create and send a TlCommand to an intersection
// action: "PREEMPT", "RECOVERY", "NORMAL"
// durationSec used for CLEAR and RECOVERY (Intersection uses it)
 void Controller::sendCmd(int interId, int approach, const char *action, double durationSec)
{
    TlCommand *cmd = new TlCommand("TlCommand");
    cmd->setIntersectionId(interId);
    cmd->setApproach(approach);
    cmd->setAction(action);
    cmd->setDuration(durationSec);//tells the intersection how long the command should last

    // Assumes vector gate: toIntersection[numIntersections]
    send(cmd, "toIntersection", interId);
}

 // startSession(): begin preemption for a winner EV
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

 // switchSessionWithClear(): HARD override with safety buffer
//  PREEMPT for pending EV
 void Controller::switchSessionWithClear(const EVState& newWinner)
{
     // End current session and immediately start the new winner.
         // The intersection's phase-aware PREEMPT logic will safely transition.
         endSessionToNormal();
         startSession(newWinner);
}

 // endSessionToNormal(): stop current session, apply recovery, return to normal
// Recovery may include previous TL if distance is below Dthreshold
 //part 3 of sdtlp
void Controller::endSessionToNormal()
{
    if (!session.active) return;

    double recoveryDuration = par("recoveryDuration").doubleValue();  //5 seconds
    double roadSegLen = par("roadSegmentLength").doubleValue();

    // Recovery at current intersection (forces preempted approach red for recoveryDuration)
    sendCmd(session.intersectionId, -1, "RECOVERY", recoveryDuration);

    // If consecutive TLs are closer than Dthreshold, also recover previous TL
    if (session.intersectionId > 0 && roadSegLen < Dthreshold) {
        sendCmd(session.intersectionId - 1, -1, "RECOVERY", recoveryDuration);
    }

     // Intersection will automatically end recovery after recoveryDuration
    // clear session
    session.active = false;
    session.evId = -1;
    session.intersectionId = -1;
    session.approach = -1;
    session.severity = 3;
    session.inClear = false;
    session.pendingEvId = -1;
}
 // applyNoPreempt(): baseline
 void Controller::applyNoPreempt()
{
    if (session.active) endSessionToNormal();
    EV_INFO << "[NO_PREEMPT] t=" << simTime()
            << " session.active=" << session.active << "\n";
}

 // applyFcfs(): baseline
// First EV that satisfies DEMV <= DD gets served; no override.
// Tie: earliest tSent (or lastSeen)

void Controller::applyFcfs()
{

     //   eligibility check = fresh + valid target + have queues + DEMV <= DD
     auto isEligible = [&](EVState& ev) -> bool {
        if (!fresh(ev.lastSeen, 1.0)) return false;
        if (ev.targetInter < 0 || ev.targetInter >= numIntersections) return false;
        if (ev.approach < 0 || ev.approach >= numApproaches) return false;
        if (!haveFreshQueues(ev.targetInter)) return false;

        double ET_sec = computeDD_dynamic(ev.targetInter, ev.approach);
        double DD_m   = ET_sec * std::max(0.1, std::fabs(ev.speed));
        return (ev.DEMV <= DD_m);
    };

     // If FCFS owner exists -> keep serving it (NO override)
    // If owner finished current intersection -> end session, release owner,
    //  then pick next winner
    // Also: if owner disappears or becomes old, treat it as finished
      if (fcfsOwner != -1) {

        //  Owner disappeared or old -> treat as FINISH
        if (evs.find(fcfsOwner) == evs.end() ||
            !fresh(evs.at(fcfsOwner).lastSeen, 1.0)) {

            int finishedId = fcfsOwner;

            if (session.active) {
                 endSessionToNormal();                           // SD-TLP Part 3 (RECOVERY)
             }

            fcfsOwner = -1;

            // fairness: let this EV re-compete later (if it still exists)
            if (evs.find(finishedId) != evs.end()) {
                evs[finishedId].hasEligibleSince = false;
            }

         }
        else {
            const EVState& owner = evs.at(fcfsOwner);

            // Start session if not active yet
            if (!session.active) {
                startSession(owner);

                // Lookahead (SD-TLP Step 2) for owner only
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

            // Owner finished intersection: targetInter changed
            if (session.evId == owner.evId &&
                session.intersectionId != owner.targetInter) {

                 int finishedId = fcfsOwner;

                endSessionToNormal();           // SD-TLP Part 3 (RECOVERY)
                fcfsOwner = -1;                 // release ownership so someone else can be chosen

                // fairness: let this EV re-compete at the next intersection
                if (evs.find(finishedId) != evs.end()) {
                    evs[finishedId].hasEligibleSince = false;
                }

              }
            else {
                // Still serving same owner at same intersection
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
        }
    }

     //   No owner (or released) -> pick FCFS winner among ELIGIBLE EVs
    // winner = smallest eligibleSince; tie -> smaller evId
     int chosen = -1;
    simtime_t bestEligibleSince = SIMTIME_ZERO;

    for (auto& kv : evs) {//loop over all EVs
        EVState& ev = kv.second;
        if (!isEligible(ev)) continue;

         if (!ev.hasEligibleSince) {//record the time the the first time an EV becomes eligible
            ev.eligibleSince = (ev.tSent >= SIMTIME_ZERO) ? ev.tSent : simTime();
            ev.hasEligibleSince = true;
        }

        if (chosen == -1 ||
            ev.eligibleSince < bestEligibleSince ||
            (ev.eligibleSince == bestEligibleSince && ev.evId < chosen)) {
            chosen = ev.evId;
            bestEligibleSince = ev.eligibleSince;
        }
    }

    // No eligible EVs -> end any active session and return
    if (chosen == -1) {
        if (session.active) endSessionToNormal();
        return;
    }


     //  Start next EV:
    fcfsOwner = chosen;

    startSession(evs.at(fcfsOwner));

    // Lookahead (SD-TLP Step 2) for the new owner
    const EVState& newOwner = evs.at(fcfsOwner);
    for (int k = 1; k <= 2; k++) {
        int nextInter = newOwner.targetInter + k;
        if (nextInter >= numIntersections) continue;
        if (!haveFreshQueues(nextInter)) continue;

        if (q[nextInter][newOwner.approach].TD > TDthreshold) {
            sendCmd(nextInter, newOwner.approach, "PREEMPT", 0.0);
        }
    }
}
 // applySdtlpSingle(): original SD-TLP single-EV behavior
  void Controller::applySdtlpSingle()
{
    int candidate = -1;

    for (const auto& kv : evs) {
        const EVState& ev = kv.second;

        if (!fresh(ev.lastSeen, 1.0)) continue;
        if (ev.targetInter < 0 || ev.targetInter >= numIntersections) continue;
        if (ev.approach < 0 || ev.approach >= numApproaches) continue;
        if (!haveFreshQueues(ev.targetInter)) continue;

        double ET_sec = computeDD_dynamic(ev.targetInter, ev.approach);
        double DD_m = ET_sec * std::max(0.1, std::fabs(ev.speed));
        // Only eligible if EV is within preemption distance
        if (ev.DEMV > DD_m) continue;
            candidate = ev.evId;
            break;   // single-EV mode: first eligible EV is enough
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

 // applySdtlpMulti(): extended SD-TLP
//  winner = severity, then EAT, then distToAP, then FCFS
// HARD override: if a different EV becomes winner (and is inside DD), switch immediately
//  Do NOT allow a losing EV to keep lookahead reservations.
 void Controller::applySdtlpMulti()
{
    int winnerId = pickWinnerEvId();

    if (winnerId == -1) {//no winner
        if (session.active) endSessionToNormal();//recovery--> part 3 of sd-tlp
        return;
    }

    const EVState& winner = evs.at(winnerId);//get state about the winner


    EV_INFO << "[MULTI_START] t=" << simTime()
            << " winner=" << winner.evId
            << " inter=" << winner.targetInter
            << " app=" << winner.approach
            << " DEMV=" << winner.DEMV
            << " C=" << q[winner.targetInter][winner.approach].C
            << "\n";
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

    // Lookahead is applied ONLY for the current global winner EV (step 2 of sdtlp)
    for (int k = 1; k <= 2; k++) {
        int nextInter = winner.targetInter + k;
        if (nextInter >= numIntersections) continue;
        if (!haveFreshQueues(nextInter)) continue;

        if (q[nextInter][winner.approach].TD > TDthreshold) {
            sendCmd(nextInter, winner.approach, "PREEMPT", 0.0);
        }
    }
}

 // handleMessage()
 void Controller::handleMessage(cMessage *msg)
{
    // QueueReport
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

    //  EvUpdate
    if (strcmp(msg->getName(), "EvUpdate") == 0) {
        EvUpdate *eu = check_and_cast<EvUpdate*>(msg);
        int id = eu->getEvId(); //new

        // Create once if new (so we do NOT erase FCFS memory fields) new
           if (evs.find(id) == evs.end()) {
               EVState tmp;
               tmp.evId = id;
               evs[id] = tmp;
           }

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

