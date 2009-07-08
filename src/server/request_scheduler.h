#ifndef REQUEST_SCHEDULER_H
#define REQUEST_SCHEDULER_H
//
// This file is part of Hecios
//
// Copyright (C) 2007,2008,2009 Brad Settlemyer
//
// This file is distributed WITHOUT ANY WARRANTY. See the file 'License.txt'
// for details on this and other legal matters.
//
#include <map>
#include <queue>
#include <omnetpp.h>
#include "pfs_types.h"
using namespace std;

/**
 * Model of a file system server request scheduler
 */
class RequestScheduler : public cSimpleModule
{
public:
    /** Constructor */
    RequestScheduler() : cSimpleModule() {};

protected:

    /** Implementation of initialize */
    virtual void initialize();

    /** Implementation of finish */
    virtual void finish();

    /** Implementation of handleMessage */
    virtual void handleMessage(cMessage* msg);

    /** Mark request complete */
    virtual void completeRequest(cMessage* msg);

    /** Schedule the next request */
    virtual void scheduleRequest(cMessage* msg);

private:

    /** In gate id */
    int requestInGateId_;

    /** Out gate id */
    int requestOutGateId_;

    /** In gate id */
    int serverInGateId_;

    /** Out gate id */
    int serverOutGateId_;

    /** Queue of messages */
    std::map<FSHandle, std::queue<cMessage*> > messageQueues_;
};

#endif

/*
 * Local variables:
 *  indent-tabs-mode: nil
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 *
 * vim: ts=8 sts=4 sw=4 expandtab
 */
