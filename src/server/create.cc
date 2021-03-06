//
// This file is part of Hecios
//
// Copyright (C) 2007,2008,2009 Brad Settlemyer
//
// This file is distributed WITHOUT ANY WARRANTY. See the file 'License.txt'
// for details on this and other legal matters.
//
#include <cassert>
#include <omnetpp.h>
#include "create.h"
#include "filename.h"
#include "fs_server.h"
#include "os_proto_m.h"
#include "pvfs_proto_m.h"
using namespace std;

Create::Create(FSServer* module, spfsCreateRequest* createReq)
    : module_(module),
      createReq_(createReq)
{
}

void Create::handleServerMessage(cMessage* msg)
{
    // Restore the existing state for this request
    cFSM currentState = createReq_->getState();

    // Server create states
    enum {
        INIT = 0,
        CREATE = FSM_Steady(1),
        FINISH = FSM_Steady(2),
    };

    FSM_Switch(currentState)
    {
    case FSM_Exit(INIT):
        {
            assert(0 != dynamic_cast<spfsCreateRequest*>(msg));
            module_->recordCreateObject();
            FSM_Goto(currentState, CREATE);
            break;
        }
    case FSM_Enter(CREATE):
        {
            assert(0 != dynamic_cast<spfsCreateRequest*>(msg));
            enterCreate();
            break;
        }
    case FSM_Exit(CREATE):
        {
            assert(0 != dynamic_cast<spfsOSFileOpenResponse*>(msg));
            module_->recordCreateObjectDiskDelay(msg);
            FSM_Goto(currentState, FINISH);
            break;
        }
    case FSM_Enter(FINISH):
        {
            assert(0 != dynamic_cast<spfsOSFileOpenResponse*>(msg));
            module_->recordCreateObjectDiskDelay(msg);
            enterFinish();
            break;
        }
    }

    // Store the state in the request
    createReq_->setState(currentState);
}

void Create::enterCreate()
{
    spfsOSFileOpenRequest* openRequest =
        new spfsOSFileOpenRequest(0, SPFS_OS_FILE_OPEN_REQUEST);
    openRequest->setContextPointer(createReq_);

    // Extract the handle as the file name
    Filename f(createReq_->getHandle());
    openRequest->setFilename(f.c_str());
    openRequest->setIsCreate(true);

    // Send the request to the storage layer
    module_->send(openRequest);
}

void Create::enterFinish()
{
    spfsCreateResponse* resp = new spfsCreateResponse(0, SPFS_CREATE_RESPONSE);
    resp->setContextPointer(createReq_);
    resp->setByteLength(4);

    // Determine the processing delay
    simtime_t delay = 0.0;
    if (SPFS_DATA_OBJECT == createReq_->getObjectType())
    {
        delay = FSServer::createDFileProcessingDelay();
    }
    else if (SPFS_DIRECTORY_OBJECT == createReq_->getObjectType())
    {
        delay = FSServer::createDirectoryProcessingDelay();
    }
    else
    {
        assert(SPFS_METADATA_OBJECT == createReq_->getObjectType());
        delay = FSServer::createMetadataProcessingDelay();
    }

    // Send the message after calculated delay
    module_->sendDelayed(resp, delay);
}

/*
 * Local variables:
 *  indent-tabs-mode: nil
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 *
 * vim: ts=4 sts=4 sw=4 expandtab cino=g0:
 */
