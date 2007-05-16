#include <iostream>
#include "mpiio_proto_m.h"
#include "umd_io_trace.h"
#include <omnetpp.h>
using namespace std;

/**
 * Model of a middleware file system data cache.
 */
class MiddlewareCache : public cSimpleModule
{
public:
    /** Constructor */
    MiddlewareCache();
    
protected:
    /** Implementation of initialize */
    virtual void initialize();

    /** Implementation of finish */
    virtual void finish() {};

    /** Implementation of handleMessage */
    virtual void handleMessage(cMessage* msg);

private:
    int appInGateId_;
    int fsInGateId_;
};

// OMNet Registriation Method
Define_Module(MiddlewareCache);

MiddlewareCache::MiddlewareCache()
{
}

void MiddlewareCache::initialize()
{
    appInGateId_ = gate("appIn")->id();
    fsInGateId_ = gate("fsIn")->id();
}

/**
 * Handle MPI-IO Response messages
 */
void MiddlewareCache::handleMessage(cMessage* msg)
{
    if (msg->arrivalGateId() == appInGateId_)
    {
        send(msg, "fsOut");
    }
    else if (msg->arrivalGateId() == fsInGateId_)
    {
        send(msg, "appOut");
    }
    else
    {
            cerr << "MiddlewareCache handleMessage: "
                 << "not yet implemented for kind: "
                 << msg->kind() << endl;
    }
}

/*
 * Local variables:
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 *
 * vim: ts=8 sts=4 sw=4 expandtab
 */
