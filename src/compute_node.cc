
#include <omnetpp.h>

/**
 * Compute Node model
 */
class ComputeNode : public cSimpleModule
{
protected:
    /**
     * Initialize the model before simulation scenario
     */
    virtual void initialize();

    /**
     * Handle an incoming message
     */
    virtual void handleMessage(cMessage* msg);
};

// OMNet Registriation Method
Define_Module(ComputeNode);

void ComputeNode::initialize()
{
    cMessage* read = new cMessage("Read some Bytes");
    send(read, "out");
}

void ComputeNode::handleMessage(cMessage *msg)
{
    ev << "Received Message '" << msg->name() << "'" << endl;
    cMessage* read_more = new cMessage("Read more bytes");
    send(read_more, "out");
    delete msg;
}
