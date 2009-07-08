//
// This file is part of Hecios
//
// Copyright (C) 2008,2009 Brad Settlemyer
//
// This file is distributed WITHOUT ANY WARRANTY. See the file 'License.txt'
// for details on this and other legal matters.
//
#include "middleware_cache.h"
#include <cassert>
#include <iostream>
#include <omnetpp.h>
#include "io_application.h"
#include "middleware_aggregator.h"
using namespace std;

MiddlewareCache::MiddlewareCache()
    : appInGateId_(-1),
      appOutGateId_(-1),
      fsInGateId_(-1),
      fsOutGateId_(-1),
      numCacheHits_(0),
      numCacheMisses_(0),
      numCacheEvicts_(0)
{
}

MiddlewareCache::~MiddlewareCache()
{
}

void MiddlewareCache::addCacheMemoryDelay(cMessage* originalRequest, double delay) const
{
    cPar* delayPar = new cPar("Delay");
    delayPar->setDoubleValue(delay);
    originalRequest->addPar(delayPar);
}

void MiddlewareCache::initialize()
{
    // Retrieve parameters
    byteCopyTime_ = par("byteCopyTime").doubleValue();

    // Find gate ids
    appInGateId_ = findGate("appIn");
    appOutGateId_ = findGate("appOut");
    fsInGateId_ = findGate("fsIn");
    fsOutGateId_ = findGate("fsOut");

    // Initialize statistics
    numCacheHits_ = 0;
    numCacheMisses_ = 0;
    numCacheEvicts_ = 0;
}

void MiddlewareCache::finish()
{
    double totalCacheAccesses = numCacheHits_ + numCacheMisses_;
    double hitRate = numCacheHits_ / totalCacheAccesses;
    recordScalar("SPFS MWare Cache Hit Rate", hitRate);
    recordScalar("SPFS MWare Cache Evictions", numCacheEvicts_);
    recordScalar("SPFS MWare Cache Hits", numCacheHits_);
    recordScalar("SPFS MWare Cache Misses", numCacheMisses_);
}

void MiddlewareCache::handleMessage(cMessage* msg)
{
     if (msg->arrivalGateId() == appInGateId())
     {
         handleApplicationMessage(msg);
     }
     else if (msg->arrivalGateId() == fsInGateId())
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

void MiddlewareCache::sendApplicationResponse(double delay, cMessage* response)
{
    // Determine the original sender
    cMessage* parentRequest = static_cast<cMessage*>(response->contextPointer());
    cModule*  parent = parentRequest->senderModule();

    // Add the delay to the message
    cPar* delayParameter = new cPar("Delay");
    *delayParameter = delay;
    response->addPar(delayParameter);

    // Locate the correct originator
    MiddlewareAggregator* originator = dynamic_cast<MiddlewareAggregator*>(parent);
    assert(0 != originator);
    originator->directMessage(response);
}


//
// NoMiddlewareCache implementation
//
//
// OMNet Registration Method
Define_Module(NoMiddlewareCache);

NoMiddlewareCache::NoMiddlewareCache()
{
}

// Perform simple pass through on all messages
void NoMiddlewareCache::handleApplicationMessage(cMessage* msg)
{
    send(msg, fsOutGateId());
}

void NoMiddlewareCache::handleFileSystemMessage(cMessage* msg)
{
    send(msg, appOutGateId());
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
