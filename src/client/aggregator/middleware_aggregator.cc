//
// This file is part of Hecios
//
// Copyright (C) 2009 Brad Settlemyer
//
// This file is distributed WITHOUT ANY WARRANTY. See the file 'License.txt'
// for details on this and other legal matters.
//
#include "middleware_aggregator.h"
#include <cassert>
#include <iostream>
#include <omnetpp.h>
#include "io_application.h"
using namespace std;

// Static variable declarations
map<cModule*, MiddlewareAggregator::CollectiveMap*> MiddlewareAggregator::sharedCollectiveMap_;
map<cModule*, MiddlewareAggregator::CollectiveRequestMap*> MiddlewareAggregator::sharedPendingRequestMap_;

// Method implementations
MiddlewareAggregator::MiddlewareAggregator()
    : cSimpleModule(),
      appInGateId_(-1),
      appOutGateId_(-1),
      ioInGateId_(-1),
      ioOutGateId_(-1)
{
}

MiddlewareAggregator::~MiddlewareAggregator()
{
}

void MiddlewareAggregator::setAggregatorSize(size_t size)
{
    aggregatorSize_ = size;
}

void MiddlewareAggregator::initialize()
{
    // Retrieve parameters
    //byteCopyTime_ = par("byteCopyTime").doubleValue();

    // Find gate ids
    appInGateId_ = findGate("appIn");
    appOutGateId_ = findGate("appOut");
    ioInGateId_ = findGate("ioIn");
    ioOutGateId_ = findGate("ioOut");

}

void MiddlewareAggregator::finish()
{
}

void MiddlewareAggregator::handleMessage(cMessage* msg)
{
    if (msg->isSelfMessage())
    {
        handleFileSystemMessage(msg);
    }
    else if (msg->getArrivalGateId() == appInGateId())
    {
        handleApplicationMessage(msg);
    }
    else if (msg->getArrivalGateId() == ioInGateId())
    {
        handleFileSystemMessage(msg);
    }
    else
    {
        cerr << __FILE__ << ":" << __LINE__ << ":"
             <<  "message arrived through illegal gate: "
             << msg->info() << endl;
    }
}

void MiddlewareAggregator::sendApplicationResponse(double delay, cMessage* response)
{
    // Determine the original sender
    cMessage* origRequest = static_cast<cMessage*>(response->getContextPointer());
    cModule* originator = origRequest->getSenderModule();

    // Add the delay to the message
    cMsgPar* delayParameter = new cMsgPar("Delay");
    *delayParameter = delay;
    response->addPar(delayParameter);

    // Locate the correct IOApplication
    IOApplication* ioApp = dynamic_cast<IOApplication*>(originator);
    assert(0 != ioApp);
    ioApp->directMessage(response);
}

void MiddlewareAggregator::directMessage(cMessage* msg)
{
    Enter_Method("Aggregator is receiving a direct message");
    take(msg);
    double delay = msg->par("Delay");
    scheduleAt(simTime() + delay, msg);
}

MiddlewareAggregator::CollectiveMap* MiddlewareAggregator::createCollectiveMap()
{
    return createSharedResource(sharedCollectiveMap_);
}

MiddlewareAggregator::CollectiveRequestMap* MiddlewareAggregator::createPendingRequestMap()
{
    return createSharedResource(sharedPendingRequestMap_);
}

template<class SharedResource>
SharedResource*
MiddlewareAggregator::createSharedResource(map<cModule*, SharedResource*>& sharedResourceMap)
{
    typedef map<cModule*, SharedResource*> SharedResourceMap;

    // Retrieve the compute node model
    cModule* cpun = findParentComputeNode();

    // If a shared resource for this node exists, return it
    // Otherwise, create and return it
    SharedResource* sharedResource = 0;
    typename SharedResourceMap::iterator iter = sharedResourceMap.find(cpun);
    if (iter != sharedResourceMap.end())
    {
        sharedResource = iter->second;
    }
    else
    {
        sharedResource = new SharedResource();
        sharedResourceMap[cpun] = sharedResource;
    }
    return sharedResource;
}

cModule* MiddlewareAggregator::findParentComputeNode() const
{
    // Extract the compute node model
    cModule* mpiProcess = getParentModule();
    assert(0 != mpiProcess);
    cModule* jobProcess = mpiProcess->getParentModule();
    assert(0 != jobProcess);
    cModule* cpun = jobProcess->getParentModule();
    assert(0 != cpun);
    return cpun;
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
