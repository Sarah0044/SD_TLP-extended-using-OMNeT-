#ifndef __RSU_H_
#define __RSU_H_

#include <omnetpp.h> //includes OMNeT++ base classes so we can use them in code
 namespace omnetpp{

class RSU : public cSimpleModule //says that RSU is an OMNeT++ simple module and it will recieve messages (from handleMessage)
{
  protected:
    virtual void handleMessage(cMessage *msg) override;
};
 }
#endif
