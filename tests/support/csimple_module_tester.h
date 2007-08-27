#ifndef C_SIMPLE_MODULE_TESTER_H
#define C_SIMPLE_MODULE_TESTER_H

#include <omnetpp.h>

class cSimpleModuleTester : public cSimulation
{
public:

    /** Create the tester for module */
    cSimpleModuleTester(const char* moduleTypeName, const char* nedFile);

    /** Destructor */
    ~cSimpleModuleTester();
    
    /** Deliver a message to the module */
    void deliverMessage(cMessage* msg, const char* inGateName);

    /** Deliver a message to the module */
    void deliverMessage(cMessage* msg, cGate* inGate);

    /**
     * Get an output message created by the module
     *   (may be called multiple times for a single input message)
     */
    cMessage* getOutputMessage();

private:

    cSimpleModule* module_;
};

#endif

/*
 * Local variables:
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 *
 * vim: ts=8 sts=4 sw=4 expandtab
 */
