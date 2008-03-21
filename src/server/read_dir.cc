//
// This file is part of Hecios
//
// Copyright (C) 2007 Brad Settlemyer
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
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
    fileRead->setExtent(0, readDirReq_->getDirEntCount());
    
    // Send the write request
    module_->send(fileRead);
}

void ReadDir::finish()
{
    spfsReadDirResponse* resp =
        new spfsReadDirResponse(0, SPFS_READ_DIR_RESPONSE);
    resp->setContextPointer(readDirReq_);

    // Calculate the response size
    resp->setByteLength(readDirReq_->getDirEntCount() * (8 + 256));
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
