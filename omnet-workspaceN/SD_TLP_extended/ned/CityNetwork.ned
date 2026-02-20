package ned;

network CityNetwork
{
    parameters:
        int numIntersections = 4; //used to create 4 intersections and RSUs
        int numEVs; // we can change it to 2 or 3 in .ini without changing the ned structure
        @display("bgb=900,400");
    submodules: //actually creates the modules we defined
        controller: Controller { //only one controller is created
            @display("p=460,150");
        }

        inter[numIntersections]: Intersection { //creates an array of Intersection modules
            parameters:
                intersectionId = index; // gives each intersection its ID equal to its array index
                @display("p=460,43");
        }

        rsu[numIntersections]: RSU { //creates an array of RSUs
            parameters:
                rsuId = index; //gives each RSU an index
                @display("p=461,234");
        }

        ev[numEVs]: EmergencyVehicle { //creates an array of EV
            parameters:
                evId = index; //gives each EV an emergency vehicle
                @display("p=334,234");
        }

    connections allowunconnected: //defines who is physically connected by gates
        // EV -> RSU
        // For now, connect every EV to RSU[0] (we will make EV choose the correct RSU later in C++).
        for i=0..numEVs-1 {
            ev[i].toRSU --> rsu[0].fromEV++;
        }

        // RSU -> Controller
        for i=0..numIntersections-1 { //connect every RSU to the controller
            rsu[i].toController --> controller.fromRSU++;
        }

        // connect controller <-> Intersections
        for i=0..numIntersections-1 {
            controller.toIntersection[i] --> inter[i].fromController;
            inter[i].toController --> controller.fromIntersection++;
        }

        // Intersection -> EV (broadcast signal state to all EVs)
for i=0..numIntersections-1 {
    inter[i].toEV++ --> ev[0].fromIntersection++;
    inter[i].toEV++ --> ev[1].fromIntersection++;
  //  inter[i].toEV++ --> ev[2].fromIntersection++;
    //inter[i].toEV++ --> ev[3].fromIntersection++;
}
// RSU -> Intersection (EvAtStopLine forwarding)
for i=0..numIntersections-1 {
    rsu[0].toIntersection++ --> inter[i].fromRSU;
}



}
