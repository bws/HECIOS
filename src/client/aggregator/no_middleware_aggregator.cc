//
// This file is part of Hecios
//
// Copyright (C) 2009 Brad Settlemyer
//
// This file is distributed WITHOUT ANY WARRANTY. See the file 'License.txt'
// for details on this and other legal matters.
//
#include "middleware_aggregator.h"
using namespace std;

/** Model of an aggregator that does simple pass through semantics */
class NoMiddlewareAggregator : public MiddlewareAggregator
{
public:
    /** Constructor */
    NoMiddlewareAggregator();

private:
    /** Forward application messages to file system */
    virtual void handleApplicationMessage(cMessage* msg);

    /** Forward application messages to file system */
    virtual void handleFileSystemMessage(cMessage* msg);
};

// OMNet Registration Method
Define_Module(NoMiddlewareAggregator);

NoMiddlewareAggregator::NoMiddlewareAggregator()
{
}

// Perform simple pass through on all messages
void NoMiddlewareAggregator::handleApplicationMessage(cMessage* msg)
{
    send(msg, ioOutGateId());
}

void NoMiddlewareAggregator::handleFileSystemMessage(cMessage* msg)
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
