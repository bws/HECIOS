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
#include "read_dir.h"
#include "filename.h"
#include "fs_server.h"
#include "os_proto_m.h"
#include "pvfs_proto_m.h"
using namespace std;

ReadDir::ReadDir(FSServer* module, spfsReadDirRequest* readDirReq)
    : module_(module),
      readDirReq_(readDirReq)
{
}

void ReadDir::handleServerMessage(cMessage* msg)
{
    // Restore the existing state for this request
    cFSM currentState = readDirReq_->getState();

    // Server lookup states
    enum {
        INIT = 0,
        READ_DIR = FSM_Steady(1),
        FINISH = FSM_Steady(2),
    };

    FSM_Switch(currentState)
    {
        case FSM_Exit(INIT):
        {
            module_->recordReadDir();
            FSM_Goto(currentState, READ_DIR);
            break;
        }
        case FSM_Enter(READ_DIR):
        {
            assert(0 != dynamic_cast<spfsReadDirRequest*>(msg));
            readDir();
            break;
        }
        case FSM_Exit(READ_DIR):
        {
            FSM_Goto(currentState, FINISH);
            break;
        }
        case FSM_Enter(FINISH):
        {
            assert(0 != dynamic_cast<spfsOSFileReadResponse*>(msg));
            module_->recordReadDirDiskDelay(msg);
            finish();
            break;
        }
    }

    // Store current state
    readDirReq_->setState(currentState);
}

void ReadDir::readDir()
{
    // Convert the handle into a local file name
    Filename filename(readDirReq_->getHandle());

    // Create the file read request
    spfsOSFileReadRequest* fileRead = new spfsOSFileReadRequest();
    fileRead->setContextPointer(readDirReq_);
    fileRead->setFilename(filename.c_str());
    fileRead->setOffsetArraySize(1);
    fileRead->setExtentArraySize(1);
    fileRead->setOffset(0, readDirReq_->getDirOffset());
    fileRead->setExtent(0, readDirReq_->getDirEntCount() * (8 + 64));

    // Send the write request
    module_->send(fileRead);
}

void ReadDir::finish()
{
    spfsReadDirResponse* resp =
        new spfsReadDirResponse(0, SPFS_READ_DIR_RESPONSE);
    resp->setContextPointer(readDirReq_);

    // Calculate the response size
    resp->setByteLength(readDirReq_->getDirEntCount() * (8 + 64));
    module_->sendDelayed(resp, FSServer::readDirProcessingDelay());
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
