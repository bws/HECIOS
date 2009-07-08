//
// This file is part of Hecios
//
// Copyright (C) 2007,2008,2009 Brad Settlemyer
//
// This file is distributed WITHOUT ANY WARRANTY. See the file 'License.txt'
// for details on this and other legal matters.
//
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
        if ((400 <= msg->kind()) && (600 > msg->kind()))
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
        if ((400 <= msg->kind()) && (600 > msg->kind()))
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
        exit(1);
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
 *  indent-tabs-mode: nil
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 *
 * vim: ts=8 sts=4 sw=4 expandtab
 */
