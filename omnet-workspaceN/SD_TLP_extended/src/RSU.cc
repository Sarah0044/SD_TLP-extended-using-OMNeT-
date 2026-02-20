#include "RSU.h"
#include "message_m.h"
using namespace omnetpp;

Define_Module(RSU);            //registers RSU as an OMNeT++ module

void RSU::handleMessage(omnetpp::cMessage *msg)
{

    if (strcmp(msg->getName(), "EvAtStopLine") == 0) {
            auto *m = check_and_cast<EvAtStopLine*>(msg);
            send(m, "toIntersection", m->getIntersectionId()); // IMPORTANT
            return;
        }

    // EV -> RSU : EvUpdate (the RSU will receive the message from EV)

    send(msg, "toController");   // RSU -> Controller  (the message is sent to the controller)
}


