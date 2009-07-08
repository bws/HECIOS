//
// This file is part of Hecios
//
// Copyright (C) 2007,2008,2009 Brad Settlemyer
//
// This file is distributed WITHOUT ANY WARRANTY. See the file 'License.txt'
// for details on this and other legal matters.
//
//#define FSM_DEBUG  // Enable FSM Debug output
#include "fs_set_attributes_sm.h"
#include <omnetpp.h>
#include "file_builder.h"
#include "fs_client.h"
#include "mpi_proto_m.h"
#include "pvfs_proto_m.h"
using namespace std;

FSSetAttributesSM::FSSetAttributesSM(const Filename& filename,
                                     spfsMPIRequest* mpiRequest,
                                     FSClient* client)
    : handle_(FileBuilder::instance().getMetaData(filename)->handle),
      mpiReq_(mpiRequest),
      client_(client)
{
    assert(0 != client_);
    assert(0 != mpiReq_);
}

bool FSSetAttributesSM::updateState(cFSM& currentState, cMessage* msg)
{
    // File system get attributes state machine states
    enum {
        INIT = 0,
        WRITE_ATTR = FSM_Steady(1),
        FINISH = FSM_Steady(2),
    };

    bool isComplete = false;
    FSM_Switch(currentState)
    {
        case FSM_Exit(INIT):
        {
            FSM_Goto(currentState, WRITE_ATTR);
            break;
        }
        case FSM_Enter(WRITE_ATTR):
        {
            writeAttributes();
            break;
        }
        case FSM_Exit(WRITE_ATTR):
        {
            FSM_Goto(currentState, FINISH);
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

void FSSetAttributesSM::writeAttributes()
{
    spfsSetAttrRequest *req =
        FSClient::createSetAttrRequest(handle_, SPFS_METADATA_OBJECT);
    req->setContextPointer(mpiReq_);
    client_->send(req, client_->getNetOutGate());
}

void FSSetAttributesSM::cacheAttributes()
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
