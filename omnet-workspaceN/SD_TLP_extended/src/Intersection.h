#ifndef __INTERSECTION_H_
#define __INTERSECTION_H_
#include <deque>
#include <omnetpp.h>
#include <vector> //allows for dynamic arrays
#include <string>
 namespace omnetpp{

class Intersection : public cSimpleModule
{
  private:
    // --- identity ---
    int intersectionId = -1; //which intersection this module represents
    int numApproaches = 4;//number of incoming directions(N,S,E,W)
    // ===== Metrics window (works for ALL methods, does NOT change EV behavior) =====



    // For multiple EVs possibly on different approaches:
    std::vector<int> rescueCountPerApproach;  // rescueCountPerApproach[a] > 0 means approach a is part of rescue route now
    // ============================================================================
    // --- specifying the method (baseline / SD-TLP / extension) ---
    std::string method;
    // Helper: pick the current rescue approach for UNION-window metrics
    int currentRescueForMetrics() const;
    // --- Baseline metrics (NO_PREEMPT only) ---
     simtime_t baselineEnd;
    // -------------------------
    // Metrics / baseline window state (NO_PREEMPT only)
     // --- NEW (NO_PREEMPT baseline window only) ---
     double stabilityExtra;          // seconds
     bool baselineActive;
      // --------------------------------------------
    // -------------------------
    int activeBaselineEVs;     // how many EVs currently active in baseline window
      int baselineRescueApproach;   // NO_PREEMPT: the approach of the EV for baseline AWT split
    // last known rescue approach (for baseline window when NO_PREEMPT)
    int lastRescueApproach;
    // --- signal timing operations ---
    simtime_t greenTime;
    simtime_t yellowTime;
    simtime_t redTime;
    simtime_t rDelay;
    cMessage *phaseTimer = nullptr; //phase timer
    int currentGreen = 0;          // which approach is currently green in NORMAL mode
    bool inYellowOrAllRed = false; // blocks discharge when true
    simtime_t phaseEnd = 0;        // when the current phase ends (The simulation time at which the current traffic-light phase should stop.)

    int baselineActiveEVs;        // NO_PREEMPT: how many EVs are currently in baseline window

    // --- traffic model ---
    simtime_t arrivalMean; //average interval arrival time  of vehicles (10s/5s/2.5s for low/med/high)
    double serviceRate = 0.5;//how many vehicles leave/exit the queue per second during green (0.5 = one vehicle every 2 seconds)
    int queueMax = 50;// max amount of vehicles in the lane/queue

    // queues per approach
    std::vector<int> queueLen;//stores the queue length for each approach, (example: queueLen[1] = East)

    // --- reporting ---
    simtime_t reportPeriod;//How often the intersection reports QueueReport messages to the controller

    // timers
    cMessage *reportTimer = nullptr;// triggers the periodic sending of QueueReport to the controller
    std::vector<cMessage*> arrivalTimers;//one timer per approach to generate vehicle arrivals (Each approach has its own timer that periodically generates arriving vehicles)

    // --- preemption state ---
    bool preemptActive = false; //if true--> means the controller has overridden normal cycling
    int preemptApproach = -1;//tells which approach is being forced green
    // >>> NEW: EV queue realism (cars ahead of EV only)
    std::vector<int> evAhead;              // per-approach: how many cars are ahead of the waiting EV
    std::vector<int> waitingEvId;          // per-approach: evId waiting at stop line (-1 if none)
    // <<< NEW

    int lastPreemptApproach = -1;   // remembers which approach was preempted
    int recoveryBlockApproach = -1; // approach forced to stay red during recovery
   struct EvWaitInfo {//the EV waiting in the qeueu info
      int evId;
       int approach;
      int severity;
     };
   std::deque<EvWaitInfo> evWaitQ;

   int findEvInWaitQ(int evId) const { //It checks whether a specific EV is already waiting at this intersection, and if so, where it is in the waiting queue.
       for (int i = 0; i < (int)evWaitQ.size(); i++)
           if (evWaitQ[i].evId == evId) return i;
       return -1;
   }


   // --- UNION metrics window state ---
   bool metricsActive;
   simtime_t metricsEnd;        // when to stop metrics after tail
   int activeEvCount;           // number of EVs currently "active" at this intersection

   bool rescueWindowActive;
   simtime_t rescueWindowEnd;
    simtime_t warmupTime = SIMTIME_ZERO;//warmup time

    // Recovery tracking for AWT
        bool recoveryActive = false;// capturing the delay caused by the disrupted traffic light synchronization.
        simtime_t recoveryEnd = 0;//capturing the delay caused by the disrupted traffic light synchronization.
        int rescueRouteApproach = -1;//Remembers which approach the EMV was on, so the module knows which queue to continue monitoring for "disorder"

        // --- metrics ---
         // AIWT members
         double waitingImposedSec = 0.0;    // Sum of cross-traffic queue lengths per second
         long totalArrivalsCross = 0;       // Denominator for AIWT

          // AWT members
          double waitingRescueRouteSec = 0.0; // Sum of rescue-route queue lengths (during preemption + recovery)
           int servedRescueVeh = 0;            // Denominator for AWT (vehicles that actually cleared the light)

            // Global stats
            long totalArrivalsAll = 0;
  protected:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
    virtual void finish() override;
};
 }
#endif
