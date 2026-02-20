#ifndef __EMERGENCY_VEHICLE_H_
#define __EMERGENCY_VEHICLE_H_

#include <omnetpp.h>
 namespace omnetpp{

class EmergencyVehicle : public cSimpleModule
{
  private:
    int evId = -1; //unique identifier for this EV (-1 meaning not initialized yet)
    int severity = 1; //priority  1 =ambulance, 2=FT, 3=police
    int approach = 0; //which direction the EV approaches from (N=0,E=1,S=2,W=3)

    std::vector<int> lastGreen;   // last known green approach per intersection
//    double stopLineEps = 5.0;
    bool waitingForGo = false;
    bool sentStopLineReq = false;
    int  waitingIntersection = -1;
     int  aheadCars = 0;          // NEW: cars still ahead of EV
    bool artRecorded = false;    // NEW: so finish() doesn’t overwrite ART


    double stoppedTime;

    double startPos = 0.0; //starting position in meters
    double destinationPos = 0.0; //destination position in meters
    double speed = 0.0;
    double pos = 0.0;// current position that updates over time (because position changes as Vehicle move)

    simtime_t startTime; //time when the EV begins moving/sending updates (When EV should START)
    simtime_t updatePeriod = 1.0; //how often the EV updates its movement and sends an EvUpdate to RSU
    simtime_t tStart; //stores the time when the EV actually starts moving (When EV ACTUALLY started)

    cMessage *tick = nullptr; //Timer that triggers EV movement

    double interPos(int id) const { return 300.0 * id; } //returns the   position of an intersection (Intersection 1 at pos 300m)
    int nextIntersection(double position, double v) const;// the next intersection traveling to

  protected:
    virtual void initialize() override;//Runs once at simulation start. Reads parameters and schedules the first timer tick
    virtual void handleMessage(cMessage *msg) override;//Runs whenever the module receives a message
    virtual void finish() override; //Runs at end of simulation. Used to clean timers and record results.
};
 }
#endif
