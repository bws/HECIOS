#ifndef C_SIMPLE_MODULE_TESTER_H
#define C_SIMPLE_MODULE_TESTER_H

#include <omnetpp.h>

class cSimpleModuleTester : public cSimulation
{
public:

    cSimpleModuleTester(cSimpleModule* module);

    void deliverMessage(cMessage* msg, cGate* inGate);

private:

    cSimpleModule* module_;
};

#endif
