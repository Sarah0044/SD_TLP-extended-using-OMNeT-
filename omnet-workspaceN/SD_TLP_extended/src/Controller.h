#ifndef __CONTROLLER_H_
#define __CONTROLLER_H_

#include <omnetpp.h>
#include <string>
#include <vector>
#include <map>
 namespace omnetpp{

// Controller receives:
//  - EvUpdate from RSUs
//  - QueueReport from Intersections
// Controller sends:
//  - TlCommand to Intersections

class Controller : public cSimpleModule
{
  private:
    // ---- parameters (from NED/ini) ----
    int numIntersections = 4;
    int numApproaches = 4;
    int fcfsOwner = -1;

    std::string method;          // "NO_PREEMPT", "FCFS", "SDTLP", "SDTLP_MULTI"
    double TDthreshold = 0.7;    // fixed paper parameter
    double Dthreshold = 150.0;   // meters (min distance between two traffic lights to see if both should be red or only current)
    simtime_t tClear = 3.0;      // all-red clearance time when switching sessions (safety buffer)

    // ---- the latest queue state for one [intersection][approach] ----
    struct QueueState {
        int C = 0;       // queue length
        double TD = 0.0; // traffic density of the specific approach
        simtime_t lastUpdate = -1;//time we last received a report for this approach
    };
    std::vector<std::vector<QueueState>> q; // size: numIntersections x numApproaches

    // ---- EV state tracked at controller ----
    struct EVState {
        int evId = -1;                 //id of ev
        int severity = 3;             // smaller = higher priority (1 highest)
        int targetInter = 0;          // which intersection it's approaching
        int approach = 0;             // N/E/S/W index
        double DEMV = 1e9;        // meters (distance to traffic light)
        double speed = 0.0;           // m/s
        simtime_t tSent = -1;         // when update was sent
        simtime_t lastSeen = -1;      // when controller received last update
        double distToAP = 1e9;  //distance to accident position
        simtime_t eligibleSince = SIMTIME_ZERO;   // first time DEMV <= DD
        bool hasEligibleSince = false;            // to store it only once

        // helper: EAT (seconds) to target stop line
        double eat() const {
            if (speed <= 0.0) return 1e9;
            return DEMV / speed;
        }
    };

    // active EVs by evId
    std::map<int, EVState> evs;

    // ---- preemption session (who currently controls signals) ----
    struct Session {
        bool active = false;
        int evId = -1;               // EV that owns the session
        int intersectionId = -1; //which intersection is being controlled
        int approach = -1;//which approach is being forced green
        int severity = 3;
        simtime_t startedAt = -1; //when the session began
        simtime_t lastCommandAt = -1;
        bool inClear = false;        // true while sending CLEAR (all-red)
        int pendingEvId = -1;        // used during CLEAR when switching to new EV (the EV we plan to switch to after the CLEAR finishes)

    } session;

    // ---- controller timer to periodically re-evaluate ----
    cMessage *tick = nullptr;
    simtime_t tickPeriod = 0.2;      // controller decision frequency

  private:
    // ----- decision helpers -----
    bool haveFreshQueues(int interId) const;
    int pickWinnerEvId() const;              // multi-ev arbitration (or single)
    void applyNoPreempt();
    void applyFcfs();
    void applySdtlpSingle();
    void applySdtlpMulti();
    double computeDD_dynamic(int targetInter, int approach) const;

    // send helpers
    void sendCmd(int interId, int approach, const char *action, double durationSec);

    // session transitions
    void startSession(const EVState& ev);
    void switchSessionWithClear(const EVState& ev); // higher priority override: CLEAR then PREEMPT
    void endSessionToNormal();

  protected:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
    virtual void finish() override;
};
 }

#endif
