#include "csimple_module_tester.h"

cSimpleModuleTester::cSimpleModuleTester(cSimpleModule* module)
    : cSimulation("ModuleTester"),
      module_(module)
{
    // Get the outbound gates and set them to suitable dummies
}

void cSimpleModuleTester::deliverMessage(cMessage* msg, cGate* inGate)
{
    // Set the message's arrival data
    msg->setArrival(module_, inGate->id());

    // Add the message to the simulator's message queue
    msgQueue.insert(msg);

    // perform the message
    doOneEvent(module_);
}

