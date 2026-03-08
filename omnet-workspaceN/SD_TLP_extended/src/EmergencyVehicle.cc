
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

    pos = startPos;//set current position

    lastGreen.assign(4, -1);//creates a vector of length 4 (for each intersection), -1 means unkown last state

    updatePeriod = SimTime(1, SIMTIME_S);    // Update period (The EV updates every 1 second)

    tStart = SimTime(-1.0);    // Not started yet

    stoppedTime = 0.0; //amount of time EV stopped

    // Queue-wait state
    waitingForGo        = false; //is it waiting? if true then yes
    sentStopLineReq     = false;// did it send EVAtStopLine? if true then yes
    waitingIntersection = -1;// which intersection its waiting at?
    aheadCars           = 0;
    artRecorded         = false;

    tick = new cMessage("evTick");
    scheduleAt(startTime, tick);
}

void EmergencyVehicle::handleMessage(cMessage *msg)
{
    // Signal state message from intersection
     if (strcmp(msg->getName(), "SignalState") == 0) {
        auto *st = check_and_cast<SignalState *>(msg);
        int id = st->getIntersectionId();
        if (id >= 0 && id < 4) {
            lastGreen[id] = st->getGreenApproach();
        }
        delete st;
        return;
    }

     // EvQueueAhead: how many cars are ahead of EV in queue
     // Only blocks EV if aheadCars > 0
    // If aheadCars == 0, EV is at front — no blocking here
    if (strcmp(msg->getName(), "EvQueueAhead") == 0) {
        EvQueueAhead *qa = check_and_cast<EvQueueAhead*>(msg);

        if (qa->getEvId() == evId) {
            waitingIntersection = qa->getIntersectionId();
            aheadCars = qa->getAhead();

            EV_INFO << "[EV " << evId << "] EvQueueAhead: "
                    << aheadCars << " cars ahead at intersection "
                    << waitingIntersection << "\n";

            // Only set waitingForGo if there are actual cars blocking the EV
            // If queue is empty, EV is free to move
            if (aheadCars > 0) {
                waitingForGo = true;
            }
            // aheadCars == 0 means no cars ahead — do not block EV here
            // red light stopping is handled in the movement block below
        }

        delete qa;
        return;
    }

     // EvGo: queue cleared AND green — EV may pass
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


     if (msg != tick) {
        delete msg;
        return;
    }

    // Mark start time for ART
    if (tStart < SimTime(0.0))
        tStart = simTime();

     // Movement logic
    int targetInter    = nextIntersection(pos, speed);
    double interX      = interPos(targetInter);
    double distToInter = std::fabs(interX - pos);

    double step        = std::fabs(speed) * updatePeriod.dbl(); //the step size of ev (based on its speed)
    double stopLineEps = step + 5.0;

    bool nearStopLine  = (distToInter <= stopLineEps); //checks if its near the front of intersection (within 20 m)

    int  greenApproach = lastGreen[targetInter];
    bool isGreenForMe  = (greenApproach == approach);

    // Reset old state if EV has moved away from intersection
    if (!nearStopLine && !waitingForGo && waitingIntersection != targetInter) {
        sentStopLineReq     = false;
        waitingIntersection = -1;
        aheadCars           = 0;
    }

     // Register at stop line (once per intersection)
    // Do NOT set waitingForGo here — let EvQueueAhead decide
    // based on how many cars are actually ahead
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

     // MOVEMENT / STOP decision
    // Three cases:
    // 1) Cars ahead in queue    -> wait for EvGo (queue realism)
    // 2) Red light, no queue    -> stop at line
    // 3) Green, no queue        -> move freely
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

     // Report position to controller
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

     // Destination reached?
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
