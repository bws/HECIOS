//
// This file is part of Hecios
//
// Copyright (C) 2007,2008,2009 Brad Settlemyer
//
// This file is distributed WITHOUT ANY WARRANTY. See the file 'License.txt'
// for details on this and other legal matters.
//
//#define FSM_DEBUG  // Enable FSM Debug output
#include "fs_get_attributes_generic_sm.h"
#include <omnetpp.h>
#include "file_builder.h"
#include "fs_client.h"
#include "mpi_proto_m.h"
#include "pvfs_proto_m.h"
using namespace std;

template<class AppRequestType>
FSGetAttributesGenericSM<AppRequestType>::FSGetAttributesGenericSM(
    const Filename& filename,
    bool calculateSize,
    AppRequestType* appRequest,
    FSClient* client)
    : handle_(FileBuilder::instance().getMetaData(filename)->handle),
      calculateSize_(calculateSize),
      appReq_(appRequest),
      client_(client)
{
    assert(0 != client_);
    assert(0 != appReq_);
}

template<class AppRequestType>
bool FSGetAttributesGenericSM<AppRequestType>::updateState(cFSM& currentState,
                                                    cMessage* msg)
{
    // File system get attributes state machine states
    enum {
        INIT = 0,
        GET_META_ATTR = FSM_Steady(1),
        GET_DATA_ATTR = FSM_Transient(2),
        COUNT_RESPONSES = FSM_Steady(3),
        FINISH = FSM_Steady(4),
    };

    bool isComplete = false;
    FSM_Switch(currentState)
    {
        case FSM_Exit(INIT):
        {
            bool isCached = isAttrCached();
            if (isCached && !calculateSize_)
            {
                FSM_Goto(currentState, FINISH);
            }
            else
            {
                FSM_Goto(currentState, GET_META_ATTR);
            }
            break;
        }
        case FSM_Enter(GET_META_ATTR):
        {
            getMetadata();
            break;
        }
        case FSM_Exit(GET_META_ATTR):
        {
            if (calculateSize_)
            {
                FSM_Goto(currentState, GET_DATA_ATTR);
            }
            else
            {
                FSM_Goto(currentState, FINISH);
            }
            break;
        }
        case FSM_Enter(GET_DATA_ATTR):
        {
            getDataAttributes();
            break;
        }
        case FSM_Exit(GET_DATA_ATTR):
        {
            FSM_Goto(currentState, COUNT_RESPONSES);
            break;
        }
        case FSM_Exit(COUNT_RESPONSES):
        {
            bool isFinished = countResponse();
            if (isFinished)
                FSM_Goto(currentState, FINISH);
            else
                FSM_Goto(currentState, COUNT_RESPONSES);
            break;
        }
        case FSM_Enter(FINISH):
        {
            cacheAttributes();
            isComplete = true;
            break;
        }
    }
    return isComplete;
}

template<class AppRequestType>
bool FSGetAttributesGenericSM<AppRequestType>::isAttrCached()
{
    FSMetaData* lookup = client_->fsState().lookupAttr(handle_);
    if (0 != lookup)
    {
        return true;
    }
    return false;
}

template<class AppRequestType>
void FSGetAttributesGenericSM<AppRequestType>::getMetadata()
{
    // Construct the request
    spfsGetAttrRequest *req =
        FSClient::createGetAttrRequest(handle_, SPFS_METADATA_OBJECT);
    req->setContextPointer(appReq_);
    client_->send(req, client_->getNetOutGate());
}

template<class AppRequestType>
void FSGetAttributesGenericSM<AppRequestType>::getDataAttributes()
{
    const FSMetaData* meta = FileBuilder::instance().getMetaData(handle_);

    for (size_t i = 0; i < meta->dataHandles.size(); i++)
    {
        spfsGetAttrRequest* req = FSClient::createGetAttrRequest(
            meta->dataHandles[i], SPFS_DATA_OBJECT);
        req->setContextPointer(appReq_);
        client_->send(req, client_->getNetOutGate());
    }

    appReq_->setRemainingResponses(meta->dataHandles.size());
}

template<class AppRequestType>
bool FSGetAttributesGenericSM<AppRequestType>::countResponse()
{
    bool isComplete = false;

    int numOutstanding = appReq_->getRemainingResponses() - 1;
    appReq_->setRemainingResponses(numOutstanding);
    if (0 == numOutstanding)
    {
        isComplete = true;
    }
    return isComplete;
}

template<class AppRequestType>
void FSGetAttributesGenericSM<AppRequestType>::cacheAttributes()
{
    const FSMetaData* attr = FileBuilder::instance().getMetaData(handle_);
    client_->fsState().insertAttr(handle_, *attr);
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
