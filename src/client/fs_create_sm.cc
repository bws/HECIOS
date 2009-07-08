//
// This file is part of Hecios
//
// Copyright (C) 2007,2008,2009 Brad Settlemyer
//
// This file is distributed WITHOUT ANY WARRANTY. See the file 'License.txt'
// for details on this and other legal matters.
//
//#define FSM_DEBUG  // Enable FSM Debug output
#include "fs_create_sm.h"
#include <omnetpp.h>
#include "file_builder.h"
#include "fs_client.h"
#include "mpi_proto_m.h"
#include "pvfs_proto_m.h"
using namespace std;

FSCreateSM::FSCreateSM(const Filename& filename,
                       spfsMPIRequest* mpiReq,
                       FSClient* client)
    : createFilename_(filename),
      mpiReq_(mpiReq),
      client_(client)
{
}

bool FSCreateSM::updateState(cFSM& currentState, cMessage* msg)
{
    /** File system collective create state machine states */
    enum {
        INIT = 0,
        CREATE_META = FSM_Steady(1),
        CREATE_DATA = FSM_Transient(2),
        COUNT_DATA_RESPONSES = FSM_Steady(3),
        WRITE_ATTR = FSM_Steady(4),
        WRITE_DIRENT = FSM_Steady(5),
        FINISH = FSM_Steady(6)
    };

    bool isComplete = false;
    FSM_Switch(currentState)
    {
        case FSM_Exit(INIT):
        {
            FSM_Goto(currentState, CREATE_META);
            break;
        }
        case FSM_Enter(CREATE_META):
        {
            createMeta();
            break;
        }
        case FSM_Exit(CREATE_META):
        {
            assert(0 != dynamic_cast<spfsCreateResponse*>(msg));
            FSM_Goto(currentState, CREATE_DATA);
            break;
        }
        case FSM_Enter(CREATE_DATA):
        {
            createDataObjects();
            break;
        }
        case FSM_Exit(CREATE_DATA):
        {
            FSM_Goto(currentState, COUNT_DATA_RESPONSES);
            break;
        }
        case FSM_Exit(COUNT_DATA_RESPONSES):
        {
            assert(0 != dynamic_cast<spfsCreateResponse*>(msg));
            countDataCreationResponse();
            if (isDataCreationComplete())
            {
                FSM_Goto(currentState, WRITE_ATTR);
            }
            else
            {
                FSM_Goto(currentState, COUNT_DATA_RESPONSES);
            }
            break;
        }
        case FSM_Enter(WRITE_ATTR):
        {
            writeAttributes();
            break;
        }
        case FSM_Exit(WRITE_ATTR):
        {
            cerr << msg->className() << " " << msg->info() << endl;
            assert(0 != dynamic_cast<spfsSetAttrResponse*>(msg));
            FSM_Goto(currentState, WRITE_DIRENT);
            break;
        }
        case FSM_Enter(WRITE_DIRENT):
        {
            createDirEnt();
            break;
        }
        case FSM_Exit(WRITE_DIRENT):
        {
            assert(0 != dynamic_cast<spfsCreateDirEntResponse*>(msg));
            FSM_Goto(currentState, FINISH);
            break;
        }
        case FSM_Enter(FINISH):
        {
            updateClientCache();
            isComplete = true;
            break;
        }
    }

    return isComplete;
}

void FSCreateSM::createMeta()
{
    // Build message to create metadata
    FSMetaData* meta = FileBuilder::instance().getMetaData(createFilename_);
    spfsCreateRequest* req =
        FSClient::createCreateRequest(meta->handle, SPFS_METADATA_OBJECT);
    req->setContextPointer(mpiReq_);
    client_->send(req, client_->getNetOutGate());
}

void FSCreateSM::createDataObjects()
{
    // Retrieve the bookkeeping information for this file
    FSMetaData* meta = FileBuilder::instance().getMetaData(createFilename_);

    // Construct the create requests for this file's data handles
    for (size_t i = 0; i < meta->dataHandles.size(); i++)
    {
        spfsCreateRequest* create =
            FSClient::createCreateRequest(meta->dataHandles[i],
                                          SPFS_DATA_OBJECT);
        create->setContextPointer(mpiReq_);
        client_->send(create, client_->getNetOutGate());
    }

    // Set the number of create responses to expect
    mpiReq_->setRemainingResponses(meta->dataHandles.size());
}

void FSCreateSM::countDataCreationResponse()
{
    // Decrement the number of create responses to expect
    int numResponses = mpiReq_->getRemainingResponses() - 1;
    mpiReq_->setRemainingResponses(numResponses);
}

bool FSCreateSM::isDataCreationComplete()
{
    int numResponses = mpiReq_->getRemainingResponses();
    if (0 == numResponses)
        return true;
    return false;
}

void FSCreateSM::writeAttributes()
{
    FSMetaData* meta = FileBuilder::instance().getMetaData(createFilename_);
    spfsSetAttrRequest* req =
        FSClient::createSetAttrRequest(meta->handle, SPFS_METADATA_OBJECT);
    req->setContextPointer(mpiReq_);
    client_->send(req, client_->getNetOutGate());
}

void FSCreateSM::createDirEnt()
{
    // Get the parent handle
    int parentIdx = createFilename_.getNumPathSegments() - 2;
    Filename parentName = createFilename_.getSegment(parentIdx);
    FSMetaData* parentMeta = FileBuilder::instance().getMetaData(parentName);

    // Construct the directory entry creation request
    spfsCreateDirEntRequest* req = FSClient::createCreateDirEntRequest(
        parentMeta->handle, createFilename_);
    req->setContextPointer(mpiReq_);
    client_->send(req, client_->getNetOutGate());
}

void FSCreateSM::updateClientCache()
{
    FSMetaData* meta = FileBuilder::instance().getMetaData(createFilename_);

    // Update the name cache
    client_->fsState().insertName(createFilename_.str(), meta->handle);

    // Update the attr cache
    client_->fsState().insertAttr(meta->handle, *meta);
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
