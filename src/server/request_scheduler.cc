#include "request_scheduler.h"
#include <cassert>
#include <iostream>
#include "pvfs_proto_m.h"
using namespace std;

// OMNet Registration Method
Define_Module(RequestScheduler);

/**
 * Initialization
 */
void RequestScheduler::initialize()
{
    requestInGateId_ = gate("requestIn")->id();
    requestOutGateId_ = gate("requestOut")->id();
    serverInGateId_ = gate("serverIn")->id();
    serverOutGateId_ = gate("serverOut")->id();
}

void RequestScheduler::finish()
{
}

/**
 *
 */
void RequestScheduler::handleMessage(cMessage* msg)
{
    if (msg->arrivalGateId() == requestInGateId_)
    {
        // Only pfs and cache messages require scheduling
        if ((500 <= msg->kind()) && (600 > msg->kind()))
        {
            scheduleRequest(msg);
        }
        else
        {
            send(msg, serverOutGateId_);
        }
    }
    else if (msg->arrivalGateId() == serverInGateId_)
    {
        // Only pfs and cache messages require completion
        if ((500 <= msg->kind()) && (600 > msg->kind()))
        {
            completeRequest(msg);
        }
        else
        {
            send(msg, requestOutGateId_);
        }
    }
    else
    {
        cerr << __FILE__ << ":" << __LINE__ << ":"
             <<  "message arrived through illegal gate: "
             << msg->info() << endl;
    }
}

void RequestScheduler::scheduleRequest(cMessage* msg)
{
    send(msg, serverOutGateId_);
}

void RequestScheduler::completeRequest(cMessage* msg)
{
    send(msg, requestOutGateId_);
}


/*
 * Local variables:
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 *
 * vim: ts=8 sts=4 sw=4 expandtab
 */
