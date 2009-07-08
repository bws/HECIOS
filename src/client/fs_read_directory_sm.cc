//
// This file is part of Hecios
//
// Copyright (C) 2007,2008,2009 Brad Settlemyer
//
// This file is distributed WITHOUT ANY WARRANTY. See the file 'License.txt'
// for details on this and other legal matters.
//
#include "fs_read_directory_sm.h"
#include <omnetpp.h>
#include "fs_client.h"
#include "mpi_proto_m.h"
#include "pvfs_proto_m.h"
using namespace std;

FSReadDirectorySM::FSReadDirectorySM(FileDescriptor* fd,
                                     size_t numEntries,
                                     spfsMPIRequest* mpiReq,
                                     FSClient* client)
    : descriptor_(fd),
      numEntries_(numEntries),
      mpiReq_(mpiReq),
      client_(client)
{
    assert(0 != descriptor_);
    assert(numEntries_ > 0);
    assert(0 != mpiReq_);
    assert(0 != client_);
}

bool FSReadDirectorySM::updateState(cFSM& currentState, cMessage* msg)
{
    /** File system read directory state machine states */
    enum {
        INIT = 0,
        READ_DIR_ENTS = FSM_Steady(1),
        FINISH = FSM_Steady(2)
    };

    bool isComplete = false;
    FSM_Switch(currentState)
    {
        case FSM_Exit(INIT):
        {
            FSM_Goto(currentState, READ_DIR_ENTS);
            break;
        }
        case FSM_Enter(READ_DIR_ENTS):
        {
            readDirEnt();
            break;
        }
        case FSM_Exit(READ_DIR_ENTS):
        {
            FSM_Goto(currentState, FINISH);
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

void FSReadDirectorySM::readDirEnt()
{
    const FSMetaData* meta = descriptor_->getMetaData();
    spfsReadDirRequest* req =
        FSClient::createReadDirRequest(meta->dataHandles[0], numEntries_);
    req->setContextPointer(mpiReq_);
    client_->send(req, client_->getNetOutGate());
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
