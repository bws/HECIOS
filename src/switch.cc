
#include <omnetpp.h>

/**
 * Network Switch model
 */
class Switch : public cSimpleModule
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
Define_Module(Switch);

void Switch::initialize()
{
}

void Switch::handleMessage(cMessage *msg)
{
    ev << "Received Message '" << msg->name() << "'" << endl;
    cMessage* service = new cMessage("1000 bytes read");
    send(service, "out");
    delete msg;
}
