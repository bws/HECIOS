//
// This file is part of Hecios
//
// Copyright (C) 2007,2008,2009 Brad Settlemyer
//
// This file is distributed WITHOUT ANY WARRANTY. See the file 'License.txt'
// for details on this and other legal matters.
//
//#define FSM_DEBUG  // Enable FSM Debug output
#include "fs_lookup_name_sm.h"
#include <omnetpp.h>
#include "file_builder.h"
#include "fs_client.h"
#include "mpi_proto_m.h"
#include "pvfs_proto_m.h"
using namespace std;

FSLookupNameSM::FSLookupNameSM(const Filename& lookupName,
                               spfsMPIRequest* mpiRequest,
                               FSClient* client)
    : lookupName_(lookupName),
      mpiReq_(mpiRequest),
      client_(client)
{
    assert(0 != client_);
    assert(0 != mpiReq_);
}

bool FSLookupNameSM::updateState(cFSM& currentState, cMessage* msg)
{
    // File system lookup name state machine states
    enum {
        INIT = 0,
        LOOKUP_HANDLE = FSM_Steady(1),
        FINISH = FSM_Steady(2),
    };

    bool isComplete = false;
    FSM_Switch(currentState)
    {
        case FSM_Exit(INIT):
        {
            FSLookupStatus status = isNameCached();
            if (SPFS_FOUND == status)
            {
                FSM_Goto(currentState, FINISH);
            }
            else
            {
                FSM_Goto(currentState, LOOKUP_HANDLE);
            }
            break;
        }
        case FSM_Enter(LOOKUP_HANDLE):
        {
            lookupHandleOnServer();
            break;
        }
        case FSM_Exit(LOOKUP_HANDLE):
        {
            assert(0 != dynamic_cast<spfsLookupPathResponse*>(msg));
            FSLookupStatus status = processLookup(
                static_cast<spfsLookupPathResponse*>(msg));
            if (SPFS_FOUND == status)
                FSM_Goto(currentState, FINISH);
            else if (SPFS_NOTFOUND == status)
                FSM_Goto(currentState, FINISH);
            else if (SPFS_PARTIAL == status)
                FSM_Goto(currentState, LOOKUP_HANDLE);
            else
            {
                cerr << __FILE__ << ":" << __LINE__ << ":"
                     << "ERROR: Name does not exist." << endl;
            }
            break;
        }
        case FSM_Enter(FINISH):
        {
            isComplete = true;
            break;
        }
    }
    return isComplete;
}

FSLookupStatus  FSLookupNameSM::isNameCached()
{
    // Perform the cache lookup
    size_t resolvedSegment = 0;
    FSHandle lookupHandle = 0;
    FSLookupStatus cacheStatus =
        client_->fsState().lookupName(lookupName_,
                                      resolvedSegment,
                                      &lookupHandle);

    // Update the number of resolved segments
    if (cacheStatus == SPFS_NOTFOUND)
    {
        // The root handle is well known even on cache misses
        mpiReq_->setNumResolvedSegments(1);
        if (1 == lookupName_.getNumPathSegments())
        {
            return SPFS_FOUND;
        }
        else
        {
            return SPFS_PARTIAL;
        }
    }
    else
    {
        mpiReq_->setNumResolvedSegments(resolvedSegment);
    }
    return cacheStatus;
}

void FSLookupNameSM::lookupHandleOnServer()
{
    // Find the first resolved handle
    int numResolvedSegments = mpiReq_->getNumResolvedSegments();
    Filename resolvedName = lookupName_.getSegment(numResolvedSegments - 1);
    //cerr << "Lookup: " << lookupName_ << endl;
    //cerr << "Resolved so far: " << resolvedName << endl;

    // Determine the handle of the resolved name
    FSHandle resolvedHandle =
        FileBuilder::instance().getMetaData(resolvedName)->handle;

    // Create the lookup request
    //cerr << "Resolved handle: " << resolvedHandle << endl;
    spfsLookupPathRequest* req = FSClient::createLookupPathRequest(
        lookupName_, resolvedHandle, numResolvedSegments);
    req->setContextPointer(mpiReq_);

    // Send the request
    client_->send(req, client_->getNetOutGate());
}

FSLookupStatus FSLookupNameSM::processLookup(spfsLookupPathResponse* lookupResponse)
{
    // Preconditions
    assert(0 < lookupResponse->getNumResolvedSegments());

    // Add the lookup results
    int numResolvedSegments = lookupResponse->getNumResolvedSegments();
    mpiReq_->setNumResolvedSegments(numResolvedSegments);

    // Determine lookup results
    FSLookupStatus lookupStatus = lookupResponse->getStatus();
    if (SPFS_FOUND == lookupStatus)
    {
        // Enter the resolved handle into the cache
        Filename resolvedName = lookupName_.getSegment(numResolvedSegments - 1);
        const FSMetaData* meta =
            FileBuilder::instance().getMetaData(resolvedName);
        client_->fsState().insertName(resolvedName.str(), meta->handle);
    }
    return lookupStatus;
}

/*
 * Local variables:
 *  indent-tabs-mode: nil
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 *
 * vim: ts=4 sts=4 sw=4 expandtab
 */
