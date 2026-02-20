package ned;

simple EmergencyVehicle
{
    parameters:
        int evId;                  // unique EV identifier
        int severity;              // 1=ambulance, 2=fire, 3=police

        double startPos @unit(m);           // starting position (0..3)
        double destinationPos @unit(m);  // final destination position
        int approach;              // approach to next intersection (0=N,1=E,2=S,3=W) (direction from which the EV approaches the next intersection)
        double startTime @unit(s);
        double speed @unit(mps);   // EV speed

        @display("i=block/process");


    gates:
        output toRSU;              // EvUpdate goes to RSU
        input fromIntersection[]; //recieves the signal state from intersection (in the real world the dirver will drive the car and move, but in simulation we need message)
}
