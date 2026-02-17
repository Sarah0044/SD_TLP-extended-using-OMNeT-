package ned;

simple Controller
{
    parameters:
        int numIntersections = 4;     //  4 intersections
        int numApproaches = 4;        //Because each intersection has 4 approaches: N/E/S/W

        string method = "SDTLP_MULTI";     // "NO_PREEMPT", "FCFS", "SDTLP", "SDTLP_MULTI"

        double TDthreshold = 0.7;     //   fixed paper parameter
        double Dthreshold @unit(m) = 150m;  //  fixed parameter (the distance between two traffic lights to decide if both should be red or only current, if less than this -->both, else only current)

        @display("i=block/cogwheel");//icon for GUI
    gates:
        input fromRSU[];              // EvUpdate messages come here (one or many RSUs) (The controller receives EvUpdate from RSUs)
        input fromIntersection[];     // QueueReport messages come here (The controller receives QueueReport from intersections)

        output toIntersection[];      // TlCommand messages go out to intersections (The controller sends TlCommand to intersections)
}
