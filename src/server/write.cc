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

#include "write.h"
#include <cassert>
#include <omnetpp.h>
#include "data_type_processor.h"
#include "file_distribution.h"
#include "filename.h"
#include "fs_server.h"
#include "os_proto_m.h"
#include "pvfs_proto_m.h"
using namespace std;

Write::Write(FSServer* module, spfsWriteRequest* writeReq)
    : module_(module),
      writeReq_(writeReq)
{
}

void Write::handleServerMessage(cMessage* msg)
{
    // Restore the existing state for this request
    cFSM currentState = writeReq_->getState();

    // Server lookup states
    enum {
        INIT = 0,
        WRITE_DATA = FSM_Steady(1),
        FINISH = FSM_Steady(2),
    };

    FSM_Switch(currentState)
    {
        case FSM_Exit(INIT):
        {
            assert(0 != dynamic_cast<spfsWriteRequest*>(msg));
            FSM_Goto(currentState, WRITE_DATA);
            break;
        }
        case FSM_Enter(WRITE_DATA):
        {
            assert(0 != dynamic_cast<spfsWriteRequest*>(msg));
            enterWriteData();
            break;
        }
        case FSM_Exit(WRITE_DATA):
        {
            FSM_Goto(currentState, FINISH);
            break;
        }
        case FSM_Enter(FINISH):
        {
            assert(0 != dynamic_cast<spfsOSFileWriteResponse*>(msg));
            enterFinish();
            break;
        }
    }

    // Store current state
    writeReq_->setState(currentState);
}

void Write::enterWriteData()
{
    // Determine the local file layout
    FileLayout layout;
    DataTypeProcessor::createFileLayoutForServer(writeReq_->getOffset(),
                                                 writeReq_->getDataType(),
                                                 writeReq_->getCount(),
                                                 *(writeReq_->getDist()),
                                                 10000000,
                                                 layout);

    // Construct the list i/o request
    spfsOSFileWriteRequest* fileWrite =
        new spfsOSFileWriteRequest(0, SPFS_OS_FILE_WRITE_REQUEST);
    Filename filename(writeReq_->getHandle());
    fileWrite->setContextPointer(writeReq_);
    fileWrite->setFilename(filename.c_str());
    fileWrite->setOffset(layout.offsets[0]);
    fileWrite->setExtent(layout.extents[0]);
    module_->send(fileWrite, "storageOut");
}

void Write::enterFinish()
{
    spfsWriteResponse* resp = new spfsWriteResponse(
        0, SPFS_WRITE_RESPONSE);
    resp->setContextPointer(writeReq_);
    module_->send(resp, "netOut");
}

/*
 * Local variables:
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 *
 * vim: ts=4 sts=4 sw=4 expandtab foldmethod=marker
 */
