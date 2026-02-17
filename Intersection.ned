package ned;

simple Intersection
{
    parameters:
        int intersectionId;          // 0..3
        int numApproaches = 4;       // N,E,S,W
        string method = "SDTLP_MULTI"; //select the simulation method -->"NO_PREEMPT", "FCFS", "SDTLP", "SDTLP_MULTI"

        double greenTime @unit(s) = 25s; //Default green phase duration for each approach.
        double yellowTime @unit(s) = 3s;//Yellow light duration.
        double redTime @unit(s) = 84s;        // Red duration for a given approach, fixed cycle (120s total)
        double r @unit(s) = 3s;               //traffic light switching delay--> also ensures no conflict (all-red time)
       double reportPeriod @unit(s) = 1s; //ntersection must periodically send QueueReport

       double arrivalMean @unit(s) = 10s; // low/med/high set in ini: 10s / 5s / 2.5s ( every how many secs will a car arrive)
        double serviceRate = 0.5;          // vehicles per second discharged/leave the lane (or queue) during green,(so 0.5 = 1 vehicle every 2 seconds)
        int queueMax = 50;                 // used to compute TD = C / queueMax , max number of vehicles in queue (it is an assumption)
         double normalSpeed @unit(mps) = 9.72mps; // 35 km/h (speed of normal cars)

        @display("i=block/segm"); //the icon in the GUI
    gates:
        input fromController;        // TlCommand -->receives messages from the Controller
        output toController;         // QueueReport -->sends messages to the Controller
}
