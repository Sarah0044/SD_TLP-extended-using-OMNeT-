/*#include "Intersection.h"
#include "message_m.h"
using namespace omnetpp;

#include <cstring>
#include <string>
#include <algorithm>

Define_Module(Intersection);

// Ensures that traffic density (TD) is between 0 and 1
static double clamp01(double x) {
    if (x < 0.0) return 0.0;
    if (x > 1.0) return 1.0;
    return x;
}

void Intersection::initialize()
{
    EV_INFO << "[CHECK] run=" << getEnvir()->getConfigEx()->getActiveRunNumber()
            << " u=" << uniform(0.0, 1.0, 0)
            << " initGreen=" << currentGreen << "\n";

    // --- Parameters ---
    intersectionId = par("intersectionId"); // id of the intersection
    numApproaches  = par("numApproaches");  // number of approaches
    method         = par("method").stdstringValue(); // the method it will follow

    greenTime    = par("greenTime");
    yellowTime   = par("yellowTime");
    redTime      = par("redTime"); // not explicitly used in this simplified model
    rDelay       = par("r");

    reportPeriod = par("reportPeriod"); // how often queue reports are sent
    arrivalMean  = par("arrivalMean");  // average vehicle arrival interval
    queueMax     = par("queueMax");      // maximum queue size

    warmupTime   = par("warmupTime");

    // >>> NEW (queue realism for EV)
    // evAhead[a] = ONLY the cars that were already in front when EV joined.
    // New arrivals after that are BEHIND the EV and must NOT increase evAhead[a].
    evAhead.assign(numApproaches, 0);
    waitingEvId.assign(numApproaches, -1);
    // <<< NEW

    // --- Service Rate Logic ---
    // serviceRate = speed / (vehicle length + minimum gap)
    double normalSpeed = par("normalSpeed").doubleValue(); // m/s
    double vehLength = 4.5;
    double minGap    = 2.5;
    serviceRate = normalSpeed / (vehLength + minGap);

    // --- Signal State ---
    currentGreen = intuniform(0, numApproaches-1);// approach starts with green is random

    inYellowOrAllRed = false;
    phaseEnd = simTime() + greenTime;

    // --- Queues & Timers ---
    queueLen.assign(numApproaches, 0);

    // Each approach has its own arrival process
    arrivalTimers.resize(numApproaches, nullptr);
    for (int a = 0; a < numApproaches; a++) {
        arrivalTimers[a] = new cMessage(("arrive_" + std::to_string(a)).c_str());
        scheduleAt(simTime() + exponential(arrivalMean.dbl()), arrivalTimers[a]);
    }

    // Periodic reporting to controller
    reportTimer = new cMessage("reportTimer");
    scheduleAt(simTime() + reportPeriod, reportTimer);

    // 1-second tick for phase update, discharge, and metric accumulation
    phaseTimer = new cMessage("phaseTimer");
    scheduleAt(simTime() + 1.0, phaseTimer);

    // --- Metrics Init ---
    waitingImposedSec = 0.0;        // AIWT numerator
    totalArrivalsCross = 0;         // AIWT denominator

    waitingRescueRouteSec = 0.0;    // AWT numerator
    servedRescueVeh = 0;            // AWT denominator

    // --- Preemption & Recovery State ---
    preemptActive = false;
    recoveryActive = false;
    recoveryEnd = SIMTIME_ZERO;

    preemptApproach = -1;
    rescueRouteApproach = -1;
}

void Intersection::handleMessage(cMessage *msg)
{
    // =========================================================
    // 1) VEHICLE ARRIVALS
    // =========================================================
    for (int a = 0; a < numApproaches; a++) {
        if (msg == arrivalTimers[a]) {
            queueLen[a]++;
            totalArrivalsAll++;

            // AIWT denominator:
            // Count vehicles that arrive on intersecting routes
            // while preemption impact is active.
            if (simTime() >= warmupTime && preemptActive) {
                // CLEAR (preemptApproach == -1): everyone is imposed
                // PREEMPT: all except rescue approach are imposed
                if (preemptApproach == -1 || a != preemptApproach) {
                    totalArrivalsCross++;
                }
            }

            scheduleAt(simTime() + exponential(arrivalMean.dbl()), arrivalTimers[a]);
            return;
        }
    }

    // =========================================================
    // 2) QUEUE REPORTING TO CONTROLLER
    // =========================================================
    if (msg == reportTimer) {
        for (int a = 0; a < numApproaches; a++) {
            QueueReport *qr = new QueueReport("QueueReport");
            qr->setIntersectionId(intersectionId);
            qr->setApproach(a);
            qr->setC(queueLen[a]);
            qr->setTD(clamp01((double)queueLen[a] / (double)queueMax));
            send(qr, "toController");
        }
        scheduleAt(simTime() + reportPeriod, reportTimer);
        return;
    }

    // =========================================================
    // 3) TRAFFIC LIGHT COMMANDS FROM CONTROLLER
    // =========================================================
    if (strcmp(msg->getName(), "TlCommand") == 0) {
        TlCommand *cmd = check_and_cast<TlCommand *>(msg);
        std::string action = cmd->getAction();

        if (action == "PREEMPT") {
            // Force green on rescue approach
            preemptActive = true;
            recoveryActive = false;
            preemptApproach = cmd->getApproach();
            rescueRouteApproach = preemptApproach;
        }
        else if (action == "CLEAR") {
            // All-red safety clearance (still counts as preemption impact)
            preemptActive = true;
            recoveryActive = false;
            preemptApproach = -1;
        }
        else if (action == "RECOVERY") {
            // Controller explicitly starts recovery phase
            preemptActive = false;
            recoveryActive = true;
            preemptApproach = -1;
            recoveryEnd = simTime() + cmd->getDuration();
        }
        else if (action == "NORMAL") {
            // Return to normal fixed-time cycling
            preemptActive = false;
            recoveryActive = false;
            preemptApproach = -1;
        }

        delete cmd;
        return;
    }

    // =========================================================
    // >>> NEW 3.5) EV ARRIVES NEAR STOP LINE (join queue snapshot)
    // =========================================================
    if (strcmp(msg->getName(), "EvAtStopLine") == 0) {
        EvAtStopLine *m = check_and_cast<EvAtStopLine*>(msg);

        // Only accept if this message is for this intersection
        if (m->getIntersectionId() == intersectionId) {

            int a = m->getApproach();

            // We support at most 1 waiting EV per approach in this simple model
            if (a >= 0 && a < numApproaches && waitingEvId[a] == -1) {

                waitingEvId[a] = m->getEvId();

                // Snapshot: cars currently in the queue are "ahead of the EV".
                // IMPORTANT: new arrivals after this are BEHIND the EV.
                evAhead[a] = queueLen[a];

                // Send initial "ahead" count to EV (so it can position behind the queue)
                EvQueueAhead *qa = new EvQueueAhead("EvQueueAhead");
                qa->setEvId(waitingEvId[a]);
                qa->setIntersectionId(intersectionId);
                qa->setApproach(a);
                qa->setAhead(evAhead[a]);

                // broadcast to all EVs (EV will ignore if evId not matching)
                for (int k = 0; k < gateSize("toEV"); k++) {
                    send(qa->dup(), "toEV", k);
                }
                delete qa;

                // If there are no cars ahead, EV is effectively at the front.
                // It can go ONLY when that approach is green (normal or preempt).
                // (So we do NOT send EvGo here unconditionally.)
            }
        }

        delete m;
        return;
    }
    // <<< NEW
    // =========================================================

    // =========================================================
    // 4) 1-SECOND TICK: METRICS + PHASE UPDATE + DISCHARGE
    // =========================================================
    if (msg == phaseTimer) {

        // --- Stop recovery if its duration has ended ---
        if (recoveryActive && simTime() >= recoveryEnd) {
            recoveryActive = false;
        }

        // --- Signal Phase Logic ---
        int currentActiveApproach = -1;

        if (!preemptActive) {
            // Normal fixed-time cycling
            if (simTime() >= phaseEnd) {
                if (!inYellowOrAllRed) {
                    inYellowOrAllRed = true;
                    phaseEnd = simTime() + yellowTime + rDelay;
                } else {
                    inYellowOrAllRed = false;
                    currentGreen = (currentGreen + 1) % numApproaches;
                    phaseEnd = simTime() + greenTime;
                }
            }
            if (!inYellowOrAllRed) {
                currentActiveApproach = currentGreen;
            }
        } else {
            // Preemption: force green or all-red
            currentActiveApproach = preemptApproach;
        }

        // --- Send signal state to EVs (used for stopping at red) ---
        for (int k = 0; k < gateSize("toEV"); k++) {
            SignalState *st = new SignalState("SignalState");
            st->setIntersectionId(intersectionId);
            st->setGreenApproach(currentActiveApproach); // -1 = all-red
            st->setPreemptActive(preemptActive);
            send(st, "toEV", k);
        }

        // --- Accumulate waiting times (vehicle-seconds) ---
        if (simTime() >= warmupTime) {
            for (int a = 0; a < numApproaches; a++) {

                // AIWT: imposed waiting on intersecting routes
                if (preemptActive) {
                    if (preemptApproach == -1 || a != preemptApproach) {
                        waitingImposedSec += queueLen[a];
                    }
                }

                // AWT: waiting on rescue route during preemption + recovery
                if (a == rescueRouteApproach) {
                    if (preemptActive || recoveryActive) {
                        waitingRescueRouteSec += queueLen[a];
                    }
                }
            }
        }

        // --- Vehicle Discharge ---
        if (currentActiveApproach >= 0 && queueLen[currentActiveApproach] > 0) {

            int served = (int)serviceRate;
            if (dblrand() < (serviceRate - served)) served++;

            served = std::min(served, queueLen[currentActiveApproach]);
            queueLen[currentActiveApproach] -= served;

            // >>> NEW: If an EV is waiting on this approach, reduce ONLY the cars ahead of it
            if (waitingEvId[currentActiveApproach] != -1) {

                if (evAhead[currentActiveApproach] > 0) {
                    int servedAhead = std::min(served, evAhead[currentActiveApproach]);
                    evAhead[currentActiveApproach] -= servedAhead;

                    // Tell EV updated ahead count (so it moves forward)
                    EvQueueAhead *qa = new EvQueueAhead("EvQueueAhead");
                    qa->setEvId(waitingEvId[currentActiveApproach]);
                    qa->setIntersectionId(intersectionId);
                    qa->setApproach(currentActiveApproach);
                    qa->setAhead(evAhead[currentActiveApproach]);

                    for (int k = 0; k < gateSize("toEV"); k++) {
                        send(qa->dup(), "toEV", k);
                    }
                    delete qa;
                }

                // If the lane ahead of EV is cleared AND approach is green -> release EV
                if (evAhead[currentActiveApproach] == 0) {

                    EvGo *go = new EvGo("EvGo");
                    go->setEvId(waitingEvId[currentActiveApproach]);
                    go->setIntersectionId(intersectionId);

                    // broadcast to all EVs (EV will ignore if evId not matching)
                    for (int k = 0; k < gateSize("toEV"); k++) {
                        send(go->dup(), "toEV", k);
                    }
                    delete go;

                    waitingEvId[currentActiveApproach] = -1;
                    // keep evAhead as 0
                }
            }
            // <<< NEW

            // AWT denominator: vehicles served on rescue route
            if (simTime() >= warmupTime &&
                currentActiveApproach == rescueRouteApproach &&
                (preemptActive || recoveryActive)) {
                servedRescueVeh += served;
            }
        }

        scheduleAt(simTime() + 1.0, phaseTimer);
        return;
    }

    delete msg;
}

void Intersection::finish()
{
    // AIWT = imposed waiting / affected vehicles
    if (totalArrivalsCross > 0)
        recordScalar("AIWT_seconds", waitingImposedSec / (double)totalArrivalsCross);
    else
        recordScalar("AIWT_seconds", 0.0);

    // AWT = rescue-route waiting / served rescue vehicles
    if (servedRescueVeh > 0)
        recordScalar("AWT_seconds", waitingRescueRouteSec / (double)servedRescueVeh);
    else
        recordScalar("AWT_seconds", 0.0);

    // Cleanup
    for (auto *t : arrivalTimers) cancelAndDelete(t);
    cancelAndDelete(reportTimer);
    cancelAndDelete(phaseTimer);
}
*/

/*
#include "Intersection.h"
#include "message_m.h"
using namespace omnetpp;

#include <cstring>
#include <string>
#include <algorithm>

Define_Module(Intersection);

// Ensures that traffic density (TD) is between 0 and 1
static double clamp01(double x) {
    if (x < 0.0) return 0.0;
    if (x > 1.0) return 1.0;
    return x;
}

void Intersection::initialize()
{
    EV_INFO << "[CHECK] run=" << getEnvir()->getConfigEx()->getActiveRunNumber()
            << " u=" << uniform(0.0, 1.0, 0)
            << " initGreen=" << currentGreen << "\n";

    // --- Parameters ---
    intersectionId = par("intersectionId");
    numApproaches  = par("numApproaches");
    method         = par("method").stdstringValue();

    greenTime    = par("greenTime");
    yellowTime   = par("yellowTime");
    redTime      = par("redTime");
    rDelay       = par("r");

    reportPeriod = par("reportPeriod");
    arrivalMean  = par("arrivalMean");
    queueMax     = par("queueMax");

    warmupTime   = par("warmupTime");

    // EV queue realism state (per approach)
    // evAhead[a] = ONLY the cars that were already in front when EV joined.
    // New arrivals after that are BEHIND the EV and must NOT increase evAhead[a].
    evAhead.assign(numApproaches, 0);
    waitingEvId.assign(numApproaches, -1);

    // --- Service Rate Logic ---
    double normalSpeed = par("normalSpeed").doubleValue();
    double vehLength = 4.5;
    double minGap    = 2.5;
    serviceRate = normalSpeed / (vehLength + minGap);

    // --- Signal State ---
    currentGreen = intuniform(0, numApproaches-1);
    inYellowOrAllRed = false;
    phaseEnd = simTime() + greenTime;

    // --- Queues & Timers ---
    queueLen.assign(numApproaches, 0);

    arrivalTimers.resize(numApproaches, nullptr);
    for (int a = 0; a < numApproaches; a++) {
        arrivalTimers[a] = new cMessage(("arrive_" + std::to_string(a)).c_str());
        scheduleAt(simTime() + exponential(arrivalMean.dbl()), arrivalTimers[a]);
    }

    reportTimer = new cMessage("reportTimer");
    scheduleAt(simTime() + reportPeriod, reportTimer);

    phaseTimer = new cMessage("phaseTimer");
    scheduleAt(simTime() + 1.0, phaseTimer);

    // --- Metrics Init ---
    waitingImposedSec = 0.0;
    totalArrivalsCross = 0;

    waitingRescueRouteSec = 0.0;
    servedRescueVeh = 0;

    // --- Preemption & Recovery State ---
    preemptActive = false;
    recoveryActive = false;
    recoveryEnd = SIMTIME_ZERO;

    preemptApproach = -1;
    rescueRouteApproach = -1;
}

void Intersection::handleMessage(cMessage *msg)
{
    // =========================================================
    // 1) VEHICLE ARRIVALS
    // =========================================================
    for (int a = 0; a < numApproaches; a++) {
        if (msg == arrivalTimers[a]) {
            queueLen[a]++;
            totalArrivalsAll++;

            if (simTime() >= warmupTime && preemptActive) {
                if (preemptApproach == -1 || a != preemptApproach) {
                    totalArrivalsCross++;
                }
            }

            scheduleAt(simTime() + exponential(arrivalMean.dbl()), arrivalTimers[a]);
            return;
        }
    }

    // =========================================================
    // 2) QUEUE REPORTING TO CONTROLLER
    // =========================================================
    if (msg == reportTimer) {
        for (int a = 0; a < numApproaches; a++) {
            QueueReport *qr = new QueueReport("QueueReport");
            qr->setIntersectionId(intersectionId);
            qr->setApproach(a);
            qr->setC(queueLen[a]);
            qr->setTD(clamp01((double)queueLen[a] / (double)queueMax));
            send(qr, "toController");
        }
        scheduleAt(simTime() + reportPeriod, reportTimer);
        return;
    }

    // =========================================================
    // 3) TRAFFIC LIGHT COMMANDS FROM CONTROLLER
    // =========================================================
    if (strcmp(msg->getName(), "TlCommand") == 0) {
        TlCommand *cmd = check_and_cast<TlCommand *>(msg);
        std::string action = cmd->getAction();

        if (action == "PREEMPT") {
            preemptActive = true;
            recoveryActive = false;
            preemptApproach = cmd->getApproach();
            rescueRouteApproach = preemptApproach;
        }
        else if (action == "CLEAR") {
            preemptActive = true;
            recoveryActive = false;
            preemptApproach = -1;
        }
        else if (action == "RECOVERY") {
            preemptActive = false;
            recoveryActive = true;
            preemptApproach = -1;
            recoveryEnd = simTime() + cmd->getDuration();
        }
        else if (action == "NORMAL") {
            preemptActive = false;
            recoveryActive = false;
            preemptApproach = -1;
        }

        delete cmd;
        return;
    }

    // =========================================================
    // 3.5) EV ARRIVES NEAR STOP LINE (join queue snapshot)
    // =========================================================
    if (strcmp(msg->getName(), "EvAtStopLine") == 0) {
        EvAtStopLine *m = check_and_cast<EvAtStopLine*>(msg);

        if (m->getIntersectionId() == intersectionId) {

            int a = m->getApproach();

            if (a >= 0 && a < numApproaches && waitingEvId[a] == -1) {

                waitingEvId[a] = m->getEvId();

                // Snapshot: cars currently in queue are ahead of EV.
                // New arrivals after this are BEHIND the EV.
                evAhead[a] = queueLen[a];

                EV_INFO << "[EV] EV " << waitingEvId[a]
                        << " registered at intersection " << intersectionId
                        << " approach " << a
                        << " with " << evAhead[a] << " cars ahead.\n";

                // Tell EV how many cars are ahead right now
                EvQueueAhead *qa = new EvQueueAhead("EvQueueAhead");
                qa->setEvId(waitingEvId[a]);
                qa->setIntersectionId(intersectionId);
                qa->setApproach(a);
                qa->setAhead(evAhead[a]);

                for (int k = 0; k < gateSize("toEV"); k++)
                    send(qa->dup(), "toEV", k);
                delete qa;

                // FIX 1: if queue is already empty AND approach is currently green
                // release EV immediately — no need to wait for discharge tick
                int currentActiveApproachNow = -1;
                if (!preemptActive) {
                    if (!inYellowOrAllRed) currentActiveApproachNow = currentGreen;
                } else {
                    currentActiveApproachNow = preemptApproach;
                }

                if (evAhead[a] == 0 && a == currentActiveApproachNow) {

                    EV_INFO << "[EV] EV " << waitingEvId[a]
                            << " released immediately (green + empty queue).\n";

                    EvGo *go = new EvGo("EvGo");
                    go->setEvId(waitingEvId[a]);
                    go->setIntersectionId(intersectionId);

                    for (int k = 0; k < gateSize("toEV"); k++)
                        send(go->dup(), "toEV", k);
                    delete go;

                    waitingEvId[a] = -1;
                }
                // else: EV waits. Phase tick will release it when:
                //   - queue ahead clears (discharge block), OR
                //   - approach turns green with empty queue (FIX 2 below)
            }
        }

        delete m;
        return;
    }

    // =========================================================
    // 4) 1-SECOND TICK: METRICS + PHASE UPDATE + DISCHARGE
    // =========================================================
    if (msg == phaseTimer) {

        // --- Stop recovery if its duration has ended ---
        if (recoveryActive && simTime() >= recoveryEnd) {
            recoveryActive = false;
        }

        // --- Signal Phase Logic ---
        int currentActiveApproach = -1;

        if (!preemptActive) {
            if (simTime() >= phaseEnd) {
                if (!inYellowOrAllRed) {
                    inYellowOrAllRed = true;
                    phaseEnd = simTime() + yellowTime + rDelay;
                } else {
                    inYellowOrAllRed = false;
                    currentGreen = (currentGreen + 1) % numApproaches;
                    phaseEnd = simTime() + greenTime;
                }
            }
            if (!inYellowOrAllRed) {
                currentActiveApproach = currentGreen;
            }
        } else {
            currentActiveApproach = preemptApproach;
        }

        // --- Send signal state to EVs ---
        for (int k = 0; k < gateSize("toEV"); k++) {
            SignalState *st = new SignalState("SignalState");
            st->setIntersectionId(intersectionId);
            st->setGreenApproach(currentActiveApproach);
            st->setPreemptActive(preemptActive);
            send(st, "toEV", k);
        }

        // --- Accumulate waiting times ---
        if (simTime() >= warmupTime) {
            for (int a = 0; a < numApproaches; a++) {

                if (preemptActive) {
                    if (preemptApproach == -1 || a != preemptApproach) {
                        waitingImposedSec += queueLen[a];
                    }
                }

                if (a == rescueRouteApproach) {
                    if (preemptActive || recoveryActive) {
                        waitingRescueRouteSec += queueLen[a];
                    }
                }
            }
        }

        // --- Vehicle Discharge (queue has cars) ---
        if (currentActiveApproach >= 0 && queueLen[currentActiveApproach] > 0) {

            int served = (int)serviceRate;
            if (dblrand() < (serviceRate - served)) served++;
            served = std::min(served, queueLen[currentActiveApproach]);
            queueLen[currentActiveApproach] -= served;

            // If EV is waiting, reduce cars ahead of it
            if (waitingEvId[currentActiveApproach] != -1) {

                if (evAhead[currentActiveApproach] > 0) {
                    int servedAhead = std::min(served, evAhead[currentActiveApproach]);
                    evAhead[currentActiveApproach] -= servedAhead;

                    // Tell EV updated count so it can creep forward
                    EvQueueAhead *qa = new EvQueueAhead("EvQueueAhead");
                    qa->setEvId(waitingEvId[currentActiveApproach]);
                    qa->setIntersectionId(intersectionId);
                    qa->setApproach(currentActiveApproach);
                    qa->setAhead(evAhead[currentActiveApproach]);

                    for (int k = 0; k < gateSize("toEV"); k++)
                        send(qa->dup(), "toEV", k);
                    delete qa;
                }

                // All cars ahead cleared -> release EV
                if (evAhead[currentActiveApproach] == 0) {

                    EV_INFO << "[EV] EV " << waitingEvId[currentActiveApproach]
                            << " released at intersection " << intersectionId
                            << " approach " << currentActiveApproach
                            << " (queue cleared).\n";

                    EvGo *go = new EvGo("EvGo");
                    go->setEvId(waitingEvId[currentActiveApproach]);
                    go->setIntersectionId(intersectionId);

                    for (int k = 0; k < gateSize("toEV"); k++)
                        send(go->dup(), "toEV", k);
                    delete go;

                    waitingEvId[currentActiveApproach] = -1;
                }
            }

            // AWT denominator
            if (simTime() >= warmupTime &&
                currentActiveApproach == rescueRouteApproach &&
                (preemptActive || recoveryActive)) {
                servedRescueVeh += served;
            }
        }
        // FIX 2: approach is green, queue is empty, EV is registered waiting
        // This handles EV that arrived during red with 0 cars ahead
        // and is now seeing green for the first time
        else if (currentActiveApproach >= 0 &&
                 queueLen[currentActiveApproach] == 0 &&
                 waitingEvId[currentActiveApproach] != -1 &&
                 evAhead[currentActiveApproach] == 0) {

            EV_INFO << "[EV] EV " << waitingEvId[currentActiveApproach]
                    << " released at intersection " << intersectionId
                    << " approach " << currentActiveApproach
                    << " (green + empty queue on tick).\n";

            EvGo *go = new EvGo("EvGo");
            go->setEvId(waitingEvId[currentActiveApproach]);
            go->setIntersectionId(intersectionId);

            for (int k = 0; k < gateSize("toEV"); k++)
                send(go->dup(), "toEV", k);
            delete go;

            waitingEvId[currentActiveApproach] = -1;
        }

        scheduleAt(simTime() + 1.0, phaseTimer);
        return;
    }

    delete msg;
}

void Intersection::finish()
{
    if (totalArrivalsCross > 0)
        recordScalar("AIWT_seconds", waitingImposedSec / (double)totalArrivalsCross);
    else
        recordScalar("AIWT_seconds", 0.0);

    if (servedRescueVeh > 0)
        recordScalar("AWT_seconds", waitingRescueRouteSec / (double)servedRescueVeh);
    else
        recordScalar("AWT_seconds", 0.0);

    for (auto *t : arrivalTimers) cancelAndDelete(t);
    cancelAndDelete(reportTimer);
    cancelAndDelete(phaseTimer);
}
*/
#include "Intersection.h"
#include "message_m.h"
using namespace omnetpp;

#include <cstring>
#include <string>
#include <algorithm>

Define_Module(Intersection);

// Ensures that traffic density (TD) is between 0 and 1
static double clamp01(double x) {
    if (x < 0.0) return 0.0;
    if (x > 1.0) return 1.0;
    return x;
}

void Intersection::initialize()
{
    EV_INFO << "[CHECK] run=" << getEnvir()->getConfigEx()->getActiveRunNumber()
            << " u=" << uniform(0.0, 1.0, 0)
            << " initGreen=" << currentGreen << "\n";

    // --- Parameters ---
    intersectionId = par("intersectionId");
    numApproaches  = par("numApproaches");
    method         = par("method").stdstringValue();

    greenTime    = par("greenTime");
    yellowTime   = par("yellowTime");
    redTime      = par("redTime");
    rDelay       = par("r");

    reportPeriod = par("reportPeriod");
    arrivalMean  = par("arrivalMean");
    queueMax     = par("queueMax");

    warmupTime   = par("warmupTime");

    // ------------------------------------------------------------------
    // Metrics window parameter (paper-style):
    // keep measuring a bit after the EV passes to capture stabilization
    // ------------------------------------------------------------------
    stabilityExtra = par("stabilityExtra").doubleValue();

    // ===== Metrics window state (works for ALL methods) =====
    metricsActive = false;
    metricsEnd = SIMTIME_ZERO;
    activeEvCount = 0;
    lastRescueApproach = -1;
    rescueCountPerApproach.assign(numApproaches, 0);
    // =======================================================

    // EV queue realism state (per approach)
    // evAhead[a] = ONLY the cars that were already in front when EV joined.
    // New arrivals after that are BEHIND the EV and must NOT increase evAhead[a].
    evAhead.assign(numApproaches, 0);
    waitingEvId.assign(numApproaches, -1);

    // --- Service Rate Logic ---
    double normalSpeed = par("normalSpeed").doubleValue();
    double vehLength = 4.5;
    double minGap    = 2.5;
    serviceRate = normalSpeed / (vehLength + minGap);

    // --- Signal State ---
    currentGreen = intuniform(0, numApproaches-1);
    inYellowOrAllRed = false;
    phaseEnd = simTime() + greenTime;

    // --- Queues & Timers ---
    queueLen.assign(numApproaches, 0);

    arrivalTimers.resize(numApproaches, nullptr);
    for (int a = 0; a < numApproaches; a++) {
        arrivalTimers[a] = new cMessage(("arrive_" + std::to_string(a)).c_str());
        scheduleAt(simTime() + exponential(arrivalMean.dbl()), arrivalTimers[a]);
    }

    reportTimer = new cMessage("reportTimer");
    scheduleAt(simTime() + reportPeriod, reportTimer);

    phaseTimer = new cMessage("phaseTimer");
    scheduleAt(simTime() + 1.0, phaseTimer);

    // --- Metrics Init ---
    waitingImposedSec = 0.0;     // AIWT numerator (vehicle-seconds on cross routes)
    totalArrivalsCross = 0;      // AIWT denominator (# vehicles that arrived on cross routes during window)

    waitingRescueRouteSec = 0.0; // AWT numerator (vehicle-seconds on rescue route)
    servedRescueVeh = 0;         // AWT denominator (# vehicles served on rescue route during window)

    // --- Preemption & Recovery State ---
    preemptActive = false;
    recoveryActive = false;
    recoveryEnd = SIMTIME_ZERO;

    preemptApproach = -1;
    rescueRouteApproach = -1; // used by your logic; metrics do NOT rely on it anymore
}

void Intersection::handleMessage(cMessage *msg)
{
    // =========================================================
    // 1) VEHICLE ARRIVALS
    // =========================================================
    for (int a = 0; a < numApproaches; a++) {
        if (msg == arrivalTimers[a]) {
            queueLen[a]++;
            totalArrivalsAll++;

            // ---------------------------------------------------------
            // AIWT denominator (paper meaning):
            // Count vehicles that ARRIVE on intersecting routes during the
            // "rescue impact window" (works for ALL methods, including NO_PREEMPT)
            // ---------------------------------------------------------
            if (simTime() >= warmupTime && metricsActive) {

                bool isRescueApproachNow = false;

                if (activeEvCount > 0) {
                    // During active EV(s), any approach currently marked rescue is not "cross"
                    isRescueApproachNow = (rescueCountPerApproach[a] > 0);
                } else {
                    // During tail (stabilityExtra after EV release), rescue route is lastRescueApproach
                    isRescueApproachNow = (lastRescueApproach != -1 && a == lastRescueApproach);
                }

                if (!isRescueApproachNow) {
                    totalArrivalsCross++;
                }
            }

            scheduleAt(simTime() + exponential(arrivalMean.dbl()), arrivalTimers[a]);
            return;
        }
    }

    // =========================================================
    // 2) QUEUE REPORTING TO CONTROLLER
    // =========================================================
    if (msg == reportTimer) {
        for (int a = 0; a < numApproaches; a++) {
            QueueReport *qr = new QueueReport("QueueReport");
            qr->setIntersectionId(intersectionId);
            qr->setApproach(a);
            qr->setC(queueLen[a]);
            qr->setTD(clamp01((double)queueLen[a] / (double)queueMax));
            send(qr, "toController");
        }
        scheduleAt(simTime() + reportPeriod, reportTimer);
        return;
    }

    // =========================================================
    // 3) TRAFFIC LIGHT COMMANDS FROM CONTROLLER
    // =========================================================
    if (strcmp(msg->getName(), "TlCommand") == 0) {
        TlCommand *cmd = check_and_cast<TlCommand *>(msg);
        std::string action = cmd->getAction();

        if (action == "PREEMPT") {
            preemptActive = true;
            recoveryActive = false;
            preemptApproach = cmd->getApproach();
            rescueRouteApproach = preemptApproach;
        }
        else if (action == "CLEAR") {
            preemptActive = true;
            recoveryActive = false;
            preemptApproach = -1;
        }
        else if (action == "RECOVERY") {
            preemptActive = false;
            recoveryActive = true;
            preemptApproach = -1;
            recoveryEnd = simTime() + cmd->getDuration();
        }
        else if (action == "NORMAL") {
            preemptActive = false;
            recoveryActive = false;
            preemptApproach = -1;
        }

        delete cmd;
        return;
    }

    // =========================================================
    // 3.5) EV ARRIVES NEAR STOP LINE (join queue snapshot)
    // =========================================================
    if (strcmp(msg->getName(), "EvAtStopLine") == 0) {
        EvAtStopLine *m = check_and_cast<EvAtStopLine*>(msg);

        if (m->getIntersectionId() == intersectionId) {

            int a = m->getApproach();

            if (a >= 0 && a < numApproaches && waitingEvId[a] == -1) {

                waitingEvId[a] = m->getEvId();

                // Snapshot: cars currently in queue are ahead of EV.
                // New arrivals after this are BEHIND the EV.
                evAhead[a] = queueLen[a];

                EV_INFO << "[EV] EV " << waitingEvId[a]
                        << " registered at intersection " << intersectionId
                        << " approach " << a
                        << " with " << evAhead[a] << " cars ahead.\n";

                // ---------------------------------------------------------
                // METRICS WINDOW START (paper meaning):
                // Rescue impact starts when EV reaches the stop line
                // (works for ALL methods, including NO_PREEMPT)
                // ---------------------------------------------------------
                if (simTime() >= warmupTime) {
                    metricsActive = true;
                    metricsEnd = SIMTIME_ZERO;     // cancel any tail end (new EV arrived)
                    activeEvCount++;
                    lastRescueApproach = a;
                    rescueCountPerApproach[a]++;   // mark this approach as rescue route during active EV
                }

                // Tell EV how many cars are ahead right now
                EvQueueAhead *qa = new EvQueueAhead("EvQueueAhead");
                qa->setEvId(waitingEvId[a]);
                qa->setIntersectionId(intersectionId);
                qa->setApproach(a);
                qa->setAhead(evAhead[a]);

                for (int k = 0; k < gateSize("toEV"); k++)
                    send(qa->dup(), "toEV", k);
                delete qa;

                // FIX 1: if queue is already empty AND approach is currently green
                // release EV immediately — no need to wait for discharge tick
                int currentActiveApproachNow = -1;
                if (!preemptActive) {
                    if (!inYellowOrAllRed) currentActiveApproachNow = currentGreen;
                } else {
                    currentActiveApproachNow = preemptApproach;
                }

                if (evAhead[a] == 0 && a == currentActiveApproachNow) {

                    EV_INFO << "[EV] EV " << waitingEvId[a]
                            << " released immediately (green + empty queue).\n";

                    EvGo *go = new EvGo("EvGo");
                    go->setEvId(waitingEvId[a]);
                    go->setIntersectionId(intersectionId);

                    for (int k = 0; k < gateSize("toEV"); k++)
                        send(go->dup(), "toEV", k);
                    delete go;

                    // ---------------------------------------------------------
                    // METRICS WINDOW: EV leaves immediately here
                    // We do NOT change EV behavior; we only update window counters
                    // ---------------------------------------------------------
                    if (simTime() >= warmupTime) {
                        rescueCountPerApproach[a] = std::max(0, rescueCountPerApproach[a] - 1);
                        activeEvCount = std::max(0, activeEvCount - 1);
                        lastRescueApproach = a;

                        // If no EV remains active, start tail window
                        if (activeEvCount == 0) {
                            metricsEnd = simTime() + stabilityExtra;
                        }
                    }

                    waitingEvId[a] = -1;
                }
                // else: EV waits. Phase tick will release it when:
                //   - queue ahead clears (discharge block), OR
                //   - approach turns green with empty queue (FIX 2 below)
            }
        }

        delete m;
        return;
    }

    // =========================================================
    // 4) 1-SECOND TICK: METRICS + PHASE UPDATE + DISCHARGE
    // =========================================================
    if (msg == phaseTimer) {

        // --- Stop recovery if its duration has ended ---
        if (recoveryActive && simTime() >= recoveryEnd) {
            recoveryActive = false;
        }

        // ---------------------------------------------------------
        // METRICS WINDOW STOP:
        // if we are in tail mode and tail time ended, stop measuring
        // ---------------------------------------------------------
        if (metricsActive &&
            metricsEnd != SIMTIME_ZERO &&
            simTime() >= metricsEnd) {

            metricsActive = false;
            metricsEnd = SIMTIME_ZERO;
            lastRescueApproach = -1;
            // rescueCountPerApproach should already be 0 if activeEvCount==0
        }

        // --- Signal Phase Logic ---
        int currentActiveApproach = -1;

        if (!preemptActive) {
            if (simTime() >= phaseEnd) {
                if (!inYellowOrAllRed) {
                    inYellowOrAllRed = true;
                    phaseEnd = simTime() + yellowTime + rDelay;
                } else {
                    inYellowOrAllRed = false;
                    currentGreen = (currentGreen + 1) % numApproaches;
                    phaseEnd = simTime() + greenTime;
                }
            }
            if (!inYellowOrAllRed) {
                currentActiveApproach = currentGreen;
            }
        } else {
            currentActiveApproach = preemptApproach;
        }

        // --- Send signal state to EVs ---
        for (int k = 0; k < gateSize("toEV"); k++) {
            SignalState *st = new SignalState("SignalState");
            st->setIntersectionId(intersectionId);
            st->setGreenApproach(currentActiveApproach);
            st->setPreemptActive(preemptActive);
            send(st, "toEV", k);
        }

        // --- Accumulate waiting times (paper meaning) ---
        // Count waiting during the rescue impact window (active EVs + tail)
        if (simTime() >= warmupTime && metricsActive) {

            for (int a = 0; a < numApproaches; a++) {

                bool isRescueApproachNow = false;

                if (activeEvCount > 0) {
                    // during active EV(s): rescue approaches are those with rescueCount>0
                    isRescueApproachNow = (rescueCountPerApproach[a] > 0);
                } else {
                    // during tail: rescue route is the last one
                    isRescueApproachNow = (lastRescueApproach != -1 && a == lastRescueApproach);
                }

                // AIWT numerator: waiting on intersecting routes
                if (!isRescueApproachNow) {
                    waitingImposedSec += queueLen[a];
                }

                // AWT numerator: waiting on rescue route
                if (isRescueApproachNow) {
                    waitingRescueRouteSec += queueLen[a];
                }
            }
        }

        // --- Vehicle Discharge (queue has cars) ---
        if (currentActiveApproach >= 0 && queueLen[currentActiveApproach] > 0) {

            int served = (int)serviceRate;
            if (dblrand() < (serviceRate - served)) served++;
            served = std::min(served, queueLen[currentActiveApproach]);
            queueLen[currentActiveApproach] -= served;

            // If EV is waiting, reduce cars ahead of it
            if (waitingEvId[currentActiveApproach] != -1) {

                if (evAhead[currentActiveApproach] > 0) {
                    int servedAhead = std::min(served, evAhead[currentActiveApproach]);
                    evAhead[currentActiveApproach] -= servedAhead;

                    // Tell EV updated count so it can creep forward
                    EvQueueAhead *qa = new EvQueueAhead("EvQueueAhead");
                    qa->setEvId(waitingEvId[currentActiveApproach]);
                    qa->setIntersectionId(intersectionId);
                    qa->setApproach(currentActiveApproach);
                    qa->setAhead(evAhead[currentActiveApproach]);

                    for (int k = 0; k < gateSize("toEV"); k++)
                        send(qa->dup(), "toEV", k);
                    delete qa;
                }

                // All cars ahead cleared -> release EV
                if (evAhead[currentActiveApproach] == 0) {

                    EV_INFO << "[EV] EV " << waitingEvId[currentActiveApproach]
                            << " released at intersection " << intersectionId
                            << " approach " << currentActiveApproach
                            << " (queue cleared).\n";

                    EvGo *go = new EvGo("EvGo");
                    go->setEvId(waitingEvId[currentActiveApproach]);
                    go->setIntersectionId(intersectionId);

                    for (int k = 0; k < gateSize("toEV"); k++)
                        send(go->dup(), "toEV", k);
                    delete go;

                    // ---------------------------------------------------------
                    // METRICS WINDOW: EV leaves here (normal case)
                    // We do NOT change EV behavior; we only update window counters
                    // ---------------------------------------------------------
                    if (simTime() >= warmupTime) {
                        rescueCountPerApproach[currentActiveApproach] =
                            std::max(0, rescueCountPerApproach[currentActiveApproach] - 1);

                        activeEvCount = std::max(0, activeEvCount - 1);
                        lastRescueApproach = currentActiveApproach;

                        // If no EV remains active, start tail window
                        if (activeEvCount == 0) {
                            metricsEnd = simTime() + stabilityExtra;
                        }
                    }

                    waitingEvId[currentActiveApproach] = -1;
                }
            }

            // AWT denominator: vehicles served on rescue route during metrics window
            if (simTime() >= warmupTime && metricsActive) {

                bool isRescueActiveApproach = false;

                if (activeEvCount > 0) {
                    isRescueActiveApproach = (rescueCountPerApproach[currentActiveApproach] > 0);
                } else {
                    isRescueActiveApproach = (lastRescueApproach != -1 && currentActiveApproach == lastRescueApproach);
                }

                if (isRescueActiveApproach) {
                    servedRescueVeh += served;
                }
            }
        }
        // FIX 2: approach is green, queue is empty, EV is registered waiting
        // This handles EV that arrived during red with 0 cars ahead
        // and is now seeing green for the first time
        else if (currentActiveApproach >= 0 &&
                 queueLen[currentActiveApproach] == 0 &&
                 waitingEvId[currentActiveApproach] != -1 &&
                 evAhead[currentActiveApproach] == 0) {

            EV_INFO << "[EV] EV " << waitingEvId[currentActiveApproach]
                    << " released at intersection " << intersectionId
                    << " approach " << currentActiveApproach
                    << " (green + empty queue on tick).\n";

            EvGo *go = new EvGo("EvGo");
            go->setEvId(waitingEvId[currentActiveApproach]);
            go->setIntersectionId(intersectionId);

            for (int k = 0; k < gateSize("toEV"); k++)
                send(go->dup(), "toEV", k);
            delete go;

            // ---------------------------------------------------------
            // METRICS WINDOW: EV leaves here (green+empty case)
            // ---------------------------------------------------------
            if (simTime() >= warmupTime) {
                rescueCountPerApproach[currentActiveApproach] =
                    std::max(0, rescueCountPerApproach[currentActiveApproach] - 1);

                activeEvCount = std::max(0, activeEvCount - 1);
                lastRescueApproach = currentActiveApproach;

                if (activeEvCount == 0) {
                    metricsEnd = simTime() + stabilityExtra;
                }
            }

            waitingEvId[currentActiveApproach] = -1;
        }

        scheduleAt(simTime() + 1.0, phaseTimer);
        return;
    }

    delete msg;
}

void Intersection::finish()
{
    // AIWT = imposed waiting / affected vehicles
    if (totalArrivalsCross > 0)
        recordScalar("AIWT_seconds", waitingImposedSec / (double)totalArrivalsCross);
    else
        recordScalar("AIWT_seconds", 0.0);

    // AWT = rescue-route waiting / served rescue vehicles
    if (servedRescueVeh > 0)
        recordScalar("AWT_seconds", waitingRescueRouteSec / (double)servedRescueVeh);
    else
        recordScalar("AWT_seconds", 0.0);

    // Cleanup
    for (auto *t : arrivalTimers) cancelAndDelete(t);
    cancelAndDelete(reportTimer);
    cancelAndDelete(phaseTimer);
}
