/*#include "EmergencyVehicle.h"
#include "message_m.h"
#include <cmath>
#include <cstring>

using namespace omnetpp;

Define_Module(EmergencyVehicle);

int EmergencyVehicle::nextIntersection(double position, double v) const
{
    // Corridor intersections at: 0, 300, 600, 900
    if (v >= 0) {
        for (int i = 0; i < 4; i++) {
            if (interPos(i) >= position)
                return i;
        }
        return 3;
    } else {
        for (int i = 3; i >= 0; i--) {
            if (interPos(i) <= position)
                return i;
        }
        return 0;
    }
}

void EmergencyVehicle::initialize()
{
    evId     = par("evId");
    severity = par("severity");
    approach = par("approach");

    startPos       = par("startPos").doubleValue();
    destinationPos = par("destinationPos").doubleValue();
    speed          = par("speed").doubleValue();
    startTime      = par("startTime");

    pos = startPos;
    lastGreen.assign(4, -1);
    // Fixed 1 second update
    updatePeriod = SimTime(1, SIMTIME_S);

    // Not started yet
    tStart = SIMTIME_ZERO - 1;

    // Initialize lastGreen to "unknown but safe" = all-red (-1)
    //for (int i = 0; i < 4; i++) lastGreen[i] = -1;

    tick = new cMessage("evTick");
    scheduleAt(startTime, tick);
}

/*
 * void EmergencyVehicle::handleMessage(cMessage *msg)
{
    // --- Receive signal state updates from intersections ---
    if (strcmp(msg->getName(), "SignalState") == 0) {
        auto *st = check_and_cast<SignalState *>(msg);
        int id = st->getIntersectionId();
        if (id >= 0 && id < 4) {
            lastGreen[id] = st->getGreenApproach(); // 0..3 or -1
        }
        delete st;
        return;
    }

    // --- We only expect our self-timer tick otherwise ---
    if (msg != tick) {
        delete msg;
        return;
    }

    // Mark actual start time (for ART)
    if (tStart < SIMTIME_ZERO)
        tStart = simTime();

    // Decide which intersection we are approaching BEFORE moving
    int targetInter = nextIntersection(pos, speed);
    double distToStop = std::fabs(interPos(targetInter) - pos);

    // Stop rule: if close to stop line and our approach is not green (or all-red)
    bool nearStopLine = (distToStop <= stopLineEps);
    int greenApproach = lastGreen[targetInter];      // -1 means all-red / unknown
    bool isGreenForMe = (greenApproach == approach);

    bool mustStop = nearStopLine && !isGreenForMe;

    // Move only if not stopped
    if (!mustStop) {
        pos += speed * updatePeriod.dbl();
    }

    // Recompute after (possible) move for reporting
    targetInter = nextIntersection(pos, speed);
    double DEMV = std::fabs(interPos(targetInter) - pos);
    double distToAP = std::fabs(destinationPos - pos);

    // Send update to RSU (every tick)
    EvUpdate *u = new EvUpdate("EvUpdate");
    u->setEvId(evId);
    u->setSeverity(severity);
    u->setTargetInter(targetInter);
    u->setApproach(approach);
    u->setDEMV(DEMV);
    u->setSpeed(speed);
    u->setTSent(simTime());
    u->setDistToAP(distToAP);
    send(u, "toRSU");

    // Stop condition: reached destination
    bool arrived = false;
    if (speed >= 0 && pos >= destinationPos) arrived = true;
    if (speed < 0 && pos <= destinationPos) arrived = true;

    if (!arrived) {
        scheduleAt(simTime() + updatePeriod, tick);
    } else {
        recordScalar("ART_seconds", (simTime() - tStart).dbl());
        cancelAndDelete(tick);
        tick = nullptr;
    }
}
*/


/*

void EmergencyVehicle::handleMessage(cMessage *msg)
{
    // --- Receive signal state updates from intersections ---
    if (strcmp(msg->getName(), "SignalState") == 0) {
        auto *st = check_and_cast<SignalState *>(msg);
        int id = st->getIntersectionId();
        if (id >= 0 && id < 4) {
            lastGreen[id] = st->getGreenApproach(); // 0..3 or -1
        }
        delete st;
        return;
    }

    // --- We only expect our self-timer tick otherwise ---
    if (msg != tick) {
        delete msg;
        return;
    }

    // Mark actual start time (for ART)
    if (tStart < SIMTIME_ZERO)
        tStart = simTime();

    // Decide which intersection we are approaching BEFORE moving
    int targetInter = nextIntersection(pos, speed);
    double distToStop = std::fabs(interPos(targetInter) - pos);
    // Stop rule: only obey the light when we are close to the stop line
        const double stopLineEps = 10.0;                 // meters (tune 5..15)
        bool nearStopLine = (distToStop <= stopLineEps);
    // Stop rule: if our approach is not green (or all-red), do NOT move
    int greenApproach = lastGreen[targetInter];      // -1 means all-red / unknown
    bool isGreenForMe = (greenApproach == approach);
    // Move rule:
        // - If far from the intersection: keep moving (do NOT stop early)
        // - If near the stop line: move only if green for me
        if (!nearStopLine || isGreenForMe) {
            pos += speed * updatePeriod.dbl();
        }
        // else: near stop line + red for me -> stay stopped

    // Recompute after (possible) move for reporting
    targetInter = nextIntersection(pos, speed);
    double DEMV = std::fabs(interPos(targetInter) - pos);
    double distToAP = std::fabs(destinationPos - pos);

    // Send update to RSU (every tick)
    EvUpdate *u = new EvUpdate("EvUpdate");
    u->setEvId(evId);
    u->setSeverity(severity);
    u->setTargetInter(targetInter);
    u->setApproach(approach);
    u->setDEMV(DEMV);
    u->setSpeed(speed);
    u->setTSent(simTime());
    u->setDistToAP(distToAP);
    send(u, "toRSU");

    // Stop condition: reached destination
    bool arrived = false;
    if (speed >= 0 && pos >= destinationPos) arrived = true;
    if (speed < 0 && pos <= destinationPos) arrived = true;

    if (!arrived) {
        scheduleAt(simTime() + updatePeriod, tick);
    } else {
        recordScalar("ART_seconds", (simTime() - tStart).dbl());
        cancelAndDelete(tick);
        tick = nullptr;
    }
}

void EmergencyVehicle::finish()
{
    if (tick) {
        cancelAndDelete(tick);
        tick = nullptr;
    }
}

*/



/*
#include "EmergencyVehicle.h"
#include "message_m.h"
#include <cmath>
#include <cstring>
#include <algorithm>

using namespace omnetpp;

Define_Module(EmergencyVehicle);

int EmergencyVehicle::nextIntersection(double position, double v) const
{
    // Corridor intersections at: 0, 300, 600, 900 meters.
    if (v >= 0) {
        for (int i = 0; i < 4; i++) {
            if (interPos(i) >= position)
                return i;
        }
        return 3;
    } else {
        for (int i = 3; i >= 0; i--) {
            if (interPos(i) <= position)
                return i;
        }
        return 0;
    }
}

void EmergencyVehicle::initialize()
{
    evId     = par("evId");
    severity = par("severity");
    approach = par("approach");

    startPos       = par("startPos").doubleValue();
    destinationPos = par("destinationPos").doubleValue();
    speed          = par("speed").doubleValue();
    startTime      = par("startTime");

    pos = startPos;

    // Signal state memory: -1 means unknown/all-red
    lastGreen.assign(4, -1);

    // Update period (keep 1s unless you tune it in .ini)
    updatePeriod = SimTime(1, SIMTIME_S);

    // Not started yet
    tStart = SIMTIME_ZERO - 1;

    // For debugging/correctness checks
    stoppedTime = 0.0;

    tick = new cMessage("evTick");
    scheduleAt(startTime, tick);
}

void EmergencyVehicle::handleMessage(cMessage *msg)
{
    // --- Receive signal state updates from intersections ---
    if (strcmp(msg->getName(), "SignalState") == 0) {
        auto *st = check_and_cast<SignalState *>(msg);
        int id = st->getIntersectionId();
        if (id >= 0 && id < 4) {
            lastGreen[id] = st->getGreenApproach(); // 0..3 or -1
        }
        delete st;
        return;
    }

    // --- We only expect our self-timer tick otherwise ---
    if (msg != tick) {
        delete msg;
        return;
    }

    // Mark actual start time (for ART)
    if (tStart < SIMTIME_ZERO)
        tStart = simTime();

    // --------------------------
    // Movement with red-light enforcement that cannot "jump" through stop line
    // --------------------------
    int targetInter = nextIntersection(pos, speed);
    double interX = interPos(targetInter);
    double distToInter = std::fabs(interX - pos);

    // Step length per tick (critical for discrete-time correctness)
    double step = std::fabs(speed) * updatePeriod.dbl();

    // Stop-zone must be >= step, otherwise the EV can jump past the stop line in one tick
    double stopLineEps = step + 5.0; // meters

    bool nearStopLine = (distToInter <= stopLineEps);

    int greenApproach = lastGreen[targetInter]; // -1 = unknown / all-red
    bool isGreenForMe = (greenApproach == approach);

    if (nearStopLine && !isGreenForMe) {
        // Red (or unknown/all-red) near stop line -> STOP and CLAMP to stop line
        const double safetyGap = 0.5; // meters before/after the stop line

        if (speed >= 0) {
            pos = std::min(pos, interX - safetyGap);
        } else {
            pos = std::max(pos, interX + safetyGap);
        }

        stoppedTime += updatePeriod.dbl();
    } else {
        // Otherwise move normally
        pos += speed * updatePeriod.dbl();
    }

    // --------------------------
    // Reporting after move/stop
    // --------------------------
    targetInter = nextIntersection(pos, speed);
    double DEMV = std::fabs(interPos(targetInter) - pos);
    double distToAP = std::fabs(destinationPos - pos);

    EvUpdate *u = new EvUpdate("EvUpdate");
    u->setEvId(evId);
    u->setSeverity(severity);
    u->setTargetInter(targetInter);
    u->setApproach(approach);
    u->setDEMV(DEMV);
    u->setSpeed(speed);
    u->setTSent(simTime());
    u->setDistToAP(distToAP);
    send(u, "toRSU");

    // --------------------------
    // Destination check
    // --------------------------
    bool arrived = false;
    if (speed >= 0 && pos >= destinationPos) arrived = true;
    if (speed < 0 && pos <= destinationPos) arrived = true;

    if (!arrived) {
        scheduleAt(simTime() + updatePeriod, tick);
    } else {
        recordScalar("ART_seconds", (simTime() - tStart).dbl());
        recordScalar("EV_stopped_seconds", stoppedTime);
        cancelAndDelete(tick);
        tick = nullptr;
    }
}

void EmergencyVehicle::finish()
{
    if (tick) {
        cancelAndDelete(tick);
        tick = nullptr;
    }
}
*/

/*#include "EmergencyVehicle.h"
#include "message_m.h"
#include <cmath>
#include <cstring>
#include <algorithm>

using namespace omnetpp;

Define_Module(EmergencyVehicle);

int EmergencyVehicle::nextIntersection(double position, double v) const
{
    // Corridor intersections at: 0, 300, 600, 900 meters.
    if (v >= 0) {
        for (int i = 0; i < 4; i++) {
            if (interPos(i) >= position)
                return i;
        }
        return 3;
    } else {
        for (int i = 3; i >= 0; i--) {
            if (interPos(i) <= position)
                return i;
        }
        return 0;
    }
}

void EmergencyVehicle::initialize()
{
    evId     = par("evId");
    severity = par("severity");
    approach = par("approach");

    startPos       = par("startPos").doubleValue();
    destinationPos = par("destinationPos").doubleValue();
    speed          = par("speed").doubleValue();
    startTime      = par("startTime");

    pos = startPos;

    // Signal state memory: -1 means unknown/all-red
    lastGreen.assign(4, -1);

    // Update period
    updatePeriod = SimTime(1, SIMTIME_S);

    // Not started yet
    tStart = SIMTIME_ZERO - 1;

    // Metrics
    stoppedTime = 0.0;

    // --------------------------
    // Queue-wait state
    // --------------------------
    waitingForGo = false;
    sentStopLineReq = false;
    waitingIntersection = -1;

    // >>> NEW
    aheadCars = 0;
    artRecorded = false;
    // <<< NEW

    tick = new cMessage("evTick");
    scheduleAt(startTime, tick);
}

void EmergencyVehicle::handleMessage(cMessage *msg)
{
    // =========================================================
    // Signal state from intersection
    // =========================================================
    if (strcmp(msg->getName(), "SignalState") == 0) {
        auto *st = check_and_cast<SignalState *>(msg);
        int id = st->getIntersectionId();
        if (id >= 0 && id < 4) {
            lastGreen[id] = st->getGreenApproach();
        }
        delete st;
        return;
    }

    // =========================================================
    // >>> NEW: Intersection tells EV how many cars are still ahead
    // =========================================================
    if (strcmp(msg->getName(), "EvQueueAhead") == 0) {
        EvQueueAhead *qa = check_and_cast<EvQueueAhead*>(msg);

        if (qa->getEvId() == evId) {
            waitingIntersection = qa->getIntersectionId();
            aheadCars = qa->getAhead();

            // If there are cars ahead, EV is queued behind them
            waitingForGo = (aheadCars > 0);
        }

        delete qa;
        return;
    }

    // =========================================================
    // EvGo: EV may pass now (front cleared and green)
    // =========================================================
    if (strcmp(msg->getName(), "EvGo") == 0) {
        EvGo *go = check_and_cast<EvGo*>(msg);

        if (go->getEvId() == evId && go->getIntersectionId() == waitingIntersection) {
            waitingForGo = false;
            sentStopLineReq = false;
            waitingIntersection = -1;
            aheadCars = 0;
        }

        delete go;
        return;
    }

    // =========================================================
    // Self tick
    // =========================================================
    if (msg != tick) {
        delete msg;
        return;
    }

    // Mark start time for ART
    if (tStart < SIMTIME_ZERO)
        tStart = simTime();

    // =========================================================
    // Movement logic
    // =========================================================
    int targetInter = nextIntersection(pos, speed);
    double interX = interPos(targetInter);
    double distToInter = std::fabs(interX - pos);

    double step = std::fabs(speed) * updatePeriod.dbl();
    double stopLineEps = step + 5.0; // prevent jump

    bool nearStopLine = (distToInter <= stopLineEps);

    int greenApproach = lastGreen[targetInter];
    bool isGreenForMe = (greenApproach == approach);

    // Reset stale state if we moved away (only if not currently queued)
    if (!nearStopLine && waitingIntersection != targetInter) {
        sentStopLineReq = false;
        waitingIntersection = -1;
        aheadCars = 0;
        waitingForGo = false;
    }

    // =========================================================
    // Tell intersection we are at the stop-line area (once)
    // (Intersection will snapshot queueLen into evAhead and start sending EvQueueAhead)
    // =========================================================
    if (nearStopLine && !sentStopLineReq) {
        EvAtStopLine *m = new EvAtStopLine("EvAtStopLine");
        m->setEvId(evId);
        m->setIntersectionId(targetInter);
        m->setApproach(approach);
        m->setSeverity(severity);
        send(m, "toRSU");

        sentStopLineReq = true;
        waitingIntersection = targetInter;
        // we will learn aheadCars from EvQueueAhead
    }

    // =========================================================
    // QUEUE REALISM:
    // If aheadCars > 0, EV is behind the remaining front queue.
    // As aheadCars decreases, EV automatically moves closer.
    // =========================================================
    const double vehLength = 4.5;
    const double minGap = 2.5;
    const double safetyGap = 0.5;

    if (waitingIntersection == targetInter && aheadCars > 0) {

        double backOffset = aheadCars * (vehLength + minGap);

        if (speed >= 0)
            pos = std::min(pos, interX - safetyGap - backOffset);
        else
            pos = std::max(pos, interX + safetyGap + backOffset);

        stoppedTime += updatePeriod.dbl();
    }
    else if (nearStopLine && !isGreenForMe) {
        // Red or all-red near stop line
        if (speed >= 0)
            pos = std::min(pos, interX - safetyGap);
        else
            pos = std::max(pos, interX + safetyGap);

        stoppedTime += updatePeriod.dbl();
    }
    else {
        // Normal movement
        pos += speed * updatePeriod.dbl();
    }

    // =========================================================
    // Report to controller
    // =========================================================
    targetInter = nextIntersection(pos, speed);
    double DEMV = std::fabs(interPos(targetInter) - pos);
    double distToAP = std::fabs(destinationPos - pos);

    EvUpdate *u = new EvUpdate("EvUpdate");
    u->setEvId(evId);
    u->setSeverity(severity);
    u->setTargetInter(targetInter);
    u->setApproach(approach);
    u->setDEMV(DEMV);
    u->setSpeed(speed);
    u->setTSent(simTime());
    u->setDistToAP(distToAP);
    send(u, "toRSU");

    // =========================================================
    // Destination reached?
    // =========================================================
    bool arrived = false;
    if (speed >= 0 && pos >= destinationPos) arrived = true;
    if (speed < 0 && pos <= destinationPos) arrived = true;

    if (!arrived) {
        scheduleAt(simTime() + updatePeriod, tick);
    } else {
        recordScalar("ART_seconds", (simTime() - tStart).dbl());
        recordScalar("EV_stopped_seconds", stoppedTime);
        artRecorded = true;

        cancelAndDelete(tick);
        tick = nullptr;
    }
}

void EmergencyVehicle::finish()
{
    // If it never reached destination, still record something for debugging
    if (!artRecorded && tStart >= SIMTIME_ZERO) {
        recordScalar("ART_seconds", (simTime() - tStart).dbl());
    }

    recordScalar("EV_stopped_seconds", stoppedTime);
    recordScalar("EV_finalPos", pos);
    recordScalar("EV_waitingForGo", (aheadCars > 0) ? 1 : 0);

    if (tick) {
        cancelAndDelete(tick);
        tick = nullptr;
    }
}
*/

#include "EmergencyVehicle.h"
#include "message_m.h"
#include <cmath>
#include <cstring>
#include <algorithm>

using namespace omnetpp;

Define_Module(EmergencyVehicle);

int EmergencyVehicle::nextIntersection(double position, double v) const
{
    // Corridor intersections at: 0, 300, 600, 900 meters.
    if (v >= 0) {
        for (int i = 0; i < 4; i++) {
            if (interPos(i) >= position)
                return i;
        }
        return 3;
    } else {
        for (int i = 3; i >= 0; i--) {
            if (interPos(i) <= position)
                return i;
        }
        return 0;
    }
}

void EmergencyVehicle::initialize()
{
    evId     = par("evId");
    severity = par("severity");
    approach = par("approach");

    startPos       = par("startPos").doubleValue();
    destinationPos = par("destinationPos").doubleValue();
    speed          = par("speed").doubleValue();
    startTime      = par("startTime");

    pos = startPos;

    // Signal state memory: -1 means unknown/all-red
    lastGreen.assign(4, -1);

    // Update period
    updatePeriod = SimTime(1, SIMTIME_S);

    // Not started yet
    tStart = SimTime(-1.0);

    // Metrics
    stoppedTime = 0.0;

    // Queue-wait state
    waitingForGo        = false;
    sentStopLineReq     = false;
    waitingIntersection = -1;
    aheadCars           = 0;
    artRecorded         = false;

    tick = new cMessage("evTick");
    scheduleAt(startTime, tick);
}

void EmergencyVehicle::handleMessage(cMessage *msg)
{
    // =========================================================
    // Signal state from intersection
    // =========================================================
    if (strcmp(msg->getName(), "SignalState") == 0) {
        auto *st = check_and_cast<SignalState *>(msg);
        int id = st->getIntersectionId();
        if (id >= 0 && id < 4) {
            lastGreen[id] = st->getGreenApproach();
        }
        delete st;
        return;
    }

    // =========================================================
    // EvQueueAhead: how many cars are ahead of EV in queue
    // Only blocks EV if aheadCars > 0
    // If aheadCars == 0, EV is at front — no blocking here
    // =========================================================
    if (strcmp(msg->getName(), "EvQueueAhead") == 0) {
        EvQueueAhead *qa = check_and_cast<EvQueueAhead*>(msg);

        if (qa->getEvId() == evId) {
            waitingIntersection = qa->getIntersectionId();
            aheadCars = qa->getAhead();

            EV_INFO << "[EV " << evId << "] EvQueueAhead: "
                    << aheadCars << " cars ahead at intersection "
                    << waitingIntersection << "\n";

            // Only set waitingForGo if there are actual cars blocking the EV
            // If queue is empty, EV is free to move (light state handled separately)
            if (aheadCars > 0) {
                waitingForGo = true;
            }
            // aheadCars == 0 means no cars ahead — do not block EV here
            // red light stopping is handled in the movement block below
        }

        delete qa;
        return;
    }

    // =========================================================
    // EvGo: queue cleared AND green — EV may pass
    // =========================================================
    if (strcmp(msg->getName(), "EvGo") == 0) {
        EvGo *go = check_and_cast<EvGo*>(msg);

        if (go->getEvId() == evId && go->getIntersectionId() == waitingIntersection) {

            EV_INFO << "[EV " << evId << "] EvGo received from intersection "
                    << waitingIntersection << " - resuming movement.\n";

            waitingForGo        = false;
            sentStopLineReq     = false;
            waitingIntersection = -1;
            aheadCars           = 0;
        }

        delete go;
        return;
    }

    // =========================================================
    // Self tick
    // =========================================================
    if (msg != tick) {
        delete msg;
        return;
    }

    // Mark start time for ART
    if (tStart < SimTime(0.0))
        tStart = simTime();

    // =========================================================
    // Movement logic
    // =========================================================
    int targetInter    = nextIntersection(pos, speed);
    double interX      = interPos(targetInter);
    double distToInter = std::fabs(interX - pos);

    double step        = std::fabs(speed) * updatePeriod.dbl();
    double stopLineEps = step + 5.0;

    bool nearStopLine  = (distToInter <= stopLineEps);

    int  greenApproach = lastGreen[targetInter];
    bool isGreenForMe  = (greenApproach == approach);

    // Reset stale state if EV has moved away from intersection
    if (!nearStopLine && !waitingForGo && waitingIntersection != targetInter) {
        sentStopLineReq     = false;
        waitingIntersection = -1;
        aheadCars           = 0;
    }

    // =========================================================
    // Register at stop line (once per intersection)
    // Do NOT set waitingForGo here — let EvQueueAhead decide
    // based on how many cars are actually ahead
    // =========================================================
    if (nearStopLine && !sentStopLineReq && !waitingForGo) {

        EV_INFO << "[EV " << evId << "] Near stop line of intersection "
                << targetInter << " (dist=" << distToInter
                << "). Sending EvAtStopLine.\n";

        EvAtStopLine *m = new EvAtStopLine("EvAtStopLine");
        m->setEvId(evId);
        m->setIntersectionId(targetInter);
        m->setApproach(approach);
        m->setSeverity(severity);
        send(m, "toRSU");

        sentStopLineReq     = true;
        waitingIntersection = targetInter;
        // waitingForGo stays false until EvQueueAhead says aheadCars > 0
        // or red light logic below kicks in
    }

    // =========================================================
    // MOVEMENT / STOP decision
    // Three cases:
    // 1) Cars ahead in queue    -> wait for EvGo (queue realism)
    // 2) Red light, no queue    -> stop at line
    // 3) Green, no queue        -> move freely
    // =========================================================
    const double vehLength = 4.5;
    const double minGap    = 2.5;
    const double safetyGap = 0.5;

    if (waitingForGo && aheadCars > 0) {
        // Cars ahead — creep forward as they are served
        double backOffset = aheadCars * (vehLength + minGap);

        if (speed >= 0)
            pos = std::min(pos, interX - safetyGap - backOffset);
        else
            pos = std::max(pos, interX + safetyGap + backOffset);

        stoppedTime += updatePeriod.dbl();

        EV_INFO << "[EV " << evId << "] Queued behind " << aheadCars
                << " cars at intersection " << waitingIntersection
                << ", pos=" << pos << "\n";
    }
    else if (nearStopLine && !isGreenForMe) {
        // Red light — stop just before the line
        if (speed >= 0)
            pos = std::min(pos, interX - safetyGap);
        else
            pos = std::max(pos, interX + safetyGap);

        stoppedTime += updatePeriod.dbl();

        EV_INFO << "[EV " << evId << "] Stopped at red, intersection "
                << targetInter << ", pos=" << pos << "\n";
    }
    else {
        // Green and no queue (or not near intersection) — move freely
        pos += speed * updatePeriod.dbl();

        EV_INFO << "[EV " << evId << "] Moving freely, pos=" << pos << "\n";
    }

    // =========================================================
    // Report position to controller
    // =========================================================
    targetInter     = nextIntersection(pos, speed);
    double DEMV     = std::fabs(interPos(targetInter) - pos);
    double distToAP = std::fabs(destinationPos - pos);

    EvUpdate *u = new EvUpdate("EvUpdate");
    u->setEvId(evId);
    u->setSeverity(severity);
    u->setTargetInter(targetInter);
    u->setApproach(approach);
    u->setDEMV(DEMV);
    u->setSpeed(speed);
    u->setTSent(simTime());
    u->setDistToAP(distToAP);
    send(u, "toRSU");

    // =========================================================
    // Destination reached?
    // =========================================================
    bool arrived = false;
    if (speed >= 0 && pos >= destinationPos) arrived = true;
    if (speed <  0 && pos <= destinationPos) arrived = true;

    if (!arrived) {
        scheduleAt(simTime() + updatePeriod, tick);
    } else {
        EV_INFO << "[EV " << evId << "] Arrived at destination "
                << destinationPos << " at t=" << simTime() << "\n";

        recordScalar("ART_seconds", (simTime() - tStart).dbl());
        recordScalar("EV_stopped_seconds", stoppedTime);
        artRecorded = true;

        cancelAndDelete(tick);
        tick = nullptr;
    }
}

void EmergencyVehicle::finish()
{
    // If EV never reached destination, record for debugging
    if (!artRecorded && tStart >= SimTime(0.0)) {
        recordScalar("ART_seconds", (simTime() - tStart).dbl());
    }

    recordScalar("EV_finalPos", pos);
    recordScalar("EV_waitingForGo", waitingForGo ? 1 : 0);

    if (tick) {
        cancelAndDelete(tick);
        tick = nullptr;
    }
}
