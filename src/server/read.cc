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

#include "read.h"
#include <cassert>
#include <numeric>
#include <omnetpp.h>
#include "data_type_layout.h"
#include "data_type_processor.h"
#include "fs_server.h"
#include "file_distribution.h"
#include "filename.h"
#include "lio_data_flow.h"
#include "os_proto_m.h"
#include "pvfs_proto_m.h"
using namespace std;

Read::Read(FSServer* module, spfsReadRequest* readReq)
    : module_(module),
      readReq_(readReq)
{
}

void Read::handleServerMessage(cMessage* msg)
{
    // Restore the existing state for this request
    cFSM currentState = readReq_->getState();

    // Server lookup states
    enum {
        INIT = 0,
        START_DATA_FLOW = FSM_Steady(1),
        FINISH = FSM_Steady(2),
    };

    FSM_Switch(currentState)
    {
        case FSM_Exit(INIT):
        {
            assert(0 != dynamic_cast<spfsReadRequest*>(msg));
            FSM_Goto(currentState, START_DATA_FLOW);
            break;
        }
        case FSM_Enter(START_DATA_FLOW):
        {
            assert(0 != dynamic_cast<spfsReadRequest*>(msg));
            enterReadData();
            break;
        }
        case FSM_Exit(START_DATA_FLOW):
        {
            FSM_Goto(currentState, FINISH);
            break;
        }
        case FSM_Enter(FINISH):
        {
            assert(0 != dynamic_cast<spfsOSFileReadResponse*>(msg));
            enterFinish();
            break;
        }
    }

    // Store current state
    readReq_->setState(currentState);
}

/*
void Read::enterStartDataFlow()
{
    // Create the data flow
    LIODataFlow* flow = new LIODataFlow(readReq_);
    
    // Register the data flow with the server
    module_->registerDataFlow(flow);
    
    // Begin the data flow
    flow->begin();
}
*/

void Read::enterReadData()
{
    // Determine the local file layout
    DataTypeLayout layout;
    DataTypeProcessor::createFileLayoutForServer(readReq_->getOffset(),
                                                 readReq_->getDataType(),
                                                 readReq_->getCount(),
                                                 *(readReq_->getDist()),
                                                 10000000,
                                                 layout);

    // Construct the list i/o request
    FileRegion fr = layout.getRegion(0);
    spfsOSFileReadRequest* fileRead =
        new spfsOSFileReadRequest(0, SPFS_OS_FILE_READ_REQUEST);
    Filename filename(readReq_->getHandle());
    fileRead->setContextPointer(readReq_);
    fileRead->setFilename(filename.c_str());
    fileRead->setOffset(fr.offset);
    fileRead->setExtent(fr.extent);
    module_->send(fileRead, "storageOut");
}

void Read::enterFinish()
{
    // Calculate the response size
    DataTypeLayout layout;
    DataTypeProcessor::createFileLayoutForServer(readReq_->getOffset(),
                                                 readReq_->getDataType(),
                                                 readReq_->getCount(),
                                                 *(readReq_->getDist()),
                                                 10000000,
                                                 layout);

    // Sum all the extents to determine total write size
    FileRegion fr = layout.getRegion(0);
    size_t reqBytes = fr.extent;
    assert(0 != reqBytes);
    
    spfsReadResponse* resp = new spfsReadResponse(
        0, SPFS_READ_RESPONSE);
    resp->setContextPointer(readReq_);
    resp->setByteLength(reqBytes);
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
