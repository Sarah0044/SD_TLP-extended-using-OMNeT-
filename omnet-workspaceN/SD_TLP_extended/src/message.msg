message EvUpdate {
    int evId; //ID of the EV
    int severity; // meaning priority: Ambulance=1, Fire=2, Police=3.
    int targetInter; // Which intersection the EV is currently approaching to know which traffic light is relevant
    int approach; //Which direction the EV is entering from --> 0=N, 1=E, 2=S, 3=W
    double DEMV; //distance (meters) from EV to the traffic light stop line
    double speed;
    simtime_t tSent;//Simulation time when the update was sent --> the time when the message is sent to the RSU
    double distToAP; // distance (meters) from EV to Accident Position (AP)
    
}

message TlCommand {
    int intersectionId;//Identifies which intersection to control
    int approach;//Specifies which incoming direction at that intersection-->Only one approach is green at a time (so it can be green) 
    string action;//tells the traffic light what kind of preemption action to perform.
    double duration;//How long the action should last
}

message QueueReport {
    int intersectionId;//Which intersection is reporting
    int approach;//Which approach (N/E/S/W) this report refers to
    int C;//the number of vehicles in front of the emergency vehicle at the intersection (queue length for an approach)
    double TD;//Traffic Density value to compare with the threshold
}
message SignalState //from intersection to ev that send the traffic signal state every second to ev.
{
    int intersectionId;
    int greenApproach;   // 0..3, or -1 for all-red
    bool preemptActive;
}
packet EvAtStopLine {//the point where the EV reaches the intersection  
    int evId;
    int intersectionId;
    int approach;
    int severity;
}

packet EvGo {//intersection says “your lane is clear, you can cross now” (from intersection to ev so the ev can move because its green and no cars infront of it)
    int evId;
    int intersectionId;
}

message EvCleared {//EV informs controller it actually crossed so the controller can end preemption safely (from ev to rsu, and the rsu will send it to controller)
    int evId;
    int intersectionId;
}
packet EvQueueAhead { //from intersection to ev to tell the ev how many cars infront of it
    int evId;
    int intersectionId;
    int approach;
    int ahead;   // cars still ahead of EV
}
