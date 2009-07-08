//
// This file is part of Hecios
//
// Copyright (C) 2007,2008,2009 Brad Settlemyer
//
// This file is distributed WITHOUT ANY WARRANTY. See the file 'License.txt'
// for details on this and other legal matters.
//
#include "lookup.h"
#include <cassert>
#include <omnetpp.h>
#include "file_builder.h"
#include "filename.h"
#include "fs_server.h"
#include "pvfs_proto_m.h"
#include "os_proto_m.h"
using namespace std;

Lookup::Lookup(FSServer* module, spfsLookupPathRequest* lookupReq)
    : module_(module),
      lookupReq_(lookupReq)
{
}

void Lookup::handleServerMessage(cMessage* msg)
{
    // Restore the existing state for this request
    cFSM currentState = lookupReq_->getState();

    // Server lookup states
    enum {
        INIT = 0,
        LOOKUP_NAME = FSM_Steady(1),
        FINISH_COMPLETE_LOOKUP = FSM_Steady(2),
        FINISH_PARTIAL_LOOKUP = FSM_Steady(3),
        FINISH_FAILED_LOOKUP = FSM_Steady(4)
    };

    FSM_Switch(currentState)
    {
        case FSM_Exit(INIT):
        {
            assert(0 != dynamic_cast<spfsLookupPathRequest*>(msg));
            module_->recordLookup();
            FSM_Goto(currentState, LOOKUP_NAME);
            break;
        }
        case FSM_Enter(LOOKUP_NAME):
        {
            lookupName();
            break;
        }
        case FSM_Exit(LOOKUP_NAME):
        {
            assert(0 != dynamic_cast<spfsOSFileReadResponse*>(msg));
            module_->recordLookupDiskDelay(msg);
            LookupStatus status = processLookupResult();

            if (LOCAL_LOOKUP_FAILED == status)
            {
                FSM_Goto(currentState, FINISH_FAILED_LOOKUP);
            }
            else if (FULL_LOOKUP_COMPLETE == status)
            {
                FSM_Goto(currentState, FINISH_COMPLETE_LOOKUP);
            }
            else if (LOCAL_LOOKUP_COMPLETE == status)
            {
                FSM_Goto(currentState, FINISH_PARTIAL_LOOKUP);
            }
            else
            {
                assert(LOCAL_LOOKUP_INCOMPLETE == status);
                FSM_Goto(currentState, LOOKUP_NAME);
            }
            break;
        }
        case FSM_Enter(FINISH_COMPLETE_LOOKUP):
        {
            assert(0 != dynamic_cast<spfsOSFileReadResponse*>(msg));
            finish(SPFS_FOUND);
            break;
        }
        case FSM_Enter(FINISH_PARTIAL_LOOKUP):
        {
            assert(0 != dynamic_cast<spfsOSFileReadResponse*>(msg));
            finish(SPFS_PARTIAL);
            break;
        }
        case FSM_Enter(FINISH_FAILED_LOOKUP):
        {
            assert(0 != dynamic_cast<spfsOSFileReadResponse*>(msg));
            finish(SPFS_NOTFOUND);
            break;
        }
    }
    // Store current state
    lookupReq_->setState(currentState);

}

void Lookup::lookupName()
{
    Filename fullName(lookupReq_->getFilename());
    int nextSegment = lookupReq_->getNumResolvedSegments();

    // Determine the location of the directory entries
    Filename parentName = fullName.getSegment(nextSegment - 1);
    FSMetaData* parentMeta = FileBuilder::instance().getMetaData(parentName);
    FSHandle parentHandle = parentMeta->dataHandles[0];

    // Create the directory entry read request
    spfsOSFileReadRequest* fileRead =
        new spfsOSFileReadRequest(0, SPFS_OS_FILE_READ_REQUEST);
    fileRead->setContextPointer(lookupReq_);
    Filename localFilename(parentHandle);
    fileRead->setFilename(localFilename.c_str());
    fileRead->setOffsetArraySize(1);
    fileRead->setExtentArraySize(1);
    fileRead->setOffset(0, 0);
    fileRead->setExtent(0, parentMeta->size);

    // Send the request
    module_->send(fileRead);

    //cerr << "Looking up: " << fullName.getSegment(nextSegment) << endl
    //     << "  Parent: " << parentName << " " << parentHandle << endl;
}

Lookup::LookupStatus Lookup::processLookupResult()
{
    // Get the next path segment (the result of this dirent lookup)
    size_t resolvedSegments = lookupReq_->getNumResolvedSegments();
    Filename fullName(lookupReq_->getFilename());
    Filename nextParent = fullName.getSegment(resolvedSegments);
    FSMetaData* nextMeta = FileBuilder::instance().getMetaData(nextParent);

    // Return the status of the next path segment
    //cerr << __FILE__ << ":" << __LINE__ << ":"
    //     << "Server resolved segs: " << resolvedSegments
    //     << " Name segs: " << fullName.getNumPathSegments() << endl;
    LookupStatus status = INVALID_LOOKUP_STATUS;
    if (0 == nextMeta)
    {
        status = LOCAL_LOOKUP_FAILED;
    }
    else
    {
        // Increment the number of resolved segments
        resolvedSegments++;
        lookupReq_->setNumResolvedSegments(resolvedSegments);

        // Increment the number of locally resolved segments
        size_t localResolutions = lookupReq_->getLocallyResolvedSegments() + 1;
        lookupReq_->setLocallyResolvedSegments(localResolutions);

        if (resolvedSegments == fullName.getNumPathSegments())
        {
            status = FULL_LOOKUP_COMPLETE;
        }
        else if (module_->handleIsLocal(nextMeta->dataHandles[0]))
        {
            status = LOCAL_LOOKUP_INCOMPLETE;
        }
        else
        {
            status = LOCAL_LOOKUP_COMPLETE;
        }
    }
    return status;
}

void Lookup::finish(FSLookupStatus lookupStatus)
{
    spfsLookupPathResponse* resp = new spfsLookupPathResponse(
        0, SPFS_LOOKUP_PATH_RESPONSE);
    resp->setContextPointer(lookupReq_);
    resp->setStatus(lookupStatus);
    resp->setNumResolvedSegments(lookupReq_->getNumResolvedSegments());

    // In the case where this full lookup is a create
    // modify the fully complete response
    if (SPFS_FOUND == lookupStatus && lookupReq_->getIsCreate())
    {
        resp->setStatus(SPFS_NOTFOUND);
    }

    // Determine the number of handles looked up
    int localResolutions = lookupReq_->getLocallyResolvedSegments();
    resp->setByteLength(4 + 8 * localResolutions);
    module_->sendDelayed(resp, FSServer::lookupPathProcessingDelay());
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
