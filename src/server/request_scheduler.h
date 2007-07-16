#ifndef REQUEST_SCHEDULER_H
#define REQUEST_SCHEDULER_H

#include <map>
#include <queue>
#include <list>
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
    virtual void finish() {};

    /** Implementation of handleMessage */
    virtual void handleMessage(cMessage* msg);

private:
    
    /** Map FSHandles to their queue of pending operations */
    // std::map<FSHandle, std::queue<cMessage*> > operationsByHandle_;
    std::map<FSHandle, std::list<PendingOperation*> > operationsByHandle_;
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
