package ned;

simple Controller
{
    parameters:

        int numIntersections = 4;     //  4 intersections
        int numApproaches = 4;        //Because each intersection has 4 approaches: N/E/S/W
        double tClear @unit(s) = default(3s); //seconds in which all red (clearing time)
        double tickPeriod @unit(s) = default(0.5s);
        double L @unit(m) = default(4.5m); //length of vehicles
        double MG @unit(m) = default(2.5m); //min gap between vehicles
        double regularSpeed @unit(mps) = default(9.72mps);
        double r @unit(s) = default(3s); // this is also all red, but used in formula
        double roadSegmentLength @unit(m) = default(300m);
        double recoveryDuration @unit(s) = default(5s); //time in recovery mode
       // bool useFixedFcfsDD = false;// who has prioirty in FCFS
       //double fixedFcfsDD @unit(m) = 150m;//added

        string method;     // "NO_PREEMPT", "FCFS", "SDTLP", "SDTLP_MULTI"

        double TDthreshold = 0.7;     //   fixed paper parameter
        double Dthreshold @unit(m) = 150m;  //  fixed parameter (the distance between two traffic lights to decide if both should be red or only current, if less than this -->both, else only current)

        @display("i=block/cogwheel");//icon for GUI
    gates:
        input fromRSU[numIntersections];              // EvUpdate messages come here (one or many RSUs) (The controller receives EvUpdate from RSUs and also EVCLEARED)
        input fromIntersection[numIntersections];     // QueueReport messages come here (The controller receives QueueReport from intersections)

        output toIntersection[numIntersections];      // TlCommand messages go out to intersections (The controller sends TlCommand to intersections)
}
