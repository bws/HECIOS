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
        START_DATA_FLOW = FSM_Transient(1),
        SEND_FINAL_RESPONSE = FSM_Steady(2),
        FINISH = FSM_Steady(3),
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
            startDataFlow();
            break;
        }
        case FSM_Exit(START_DATA_FLOW):
        {
            FSM_Goto(currentState, SEND_FINAL_RESPONSE);
            break;
        }
        case FSM_Enter(SEND_FINAL_RESPONSE):
        {
            assert(0 != dynamic_cast<spfsReadRequest*>(msg));
            sendFinalResponse();
            break;
        }
        case FSM_Exit(SEND_FINAL_RESPONSE):
        {
            FSM_Goto(currentState, FINISH);
            break;
        }
        case FSM_Enter(FINISH):
        {
            assert(0 != dynamic_cast<spfsDataFlowFinish*>(msg));
            finish();
            break;
        }
    }

    // Store current state
    readReq_->setState(currentState);
}

void Read::startDataFlow()
{
    // Construct the data flow establish message
    spfsDataFlowStart* dataFlowStart =
        new spfsDataFlowStart(0, SPFS_DATA_FLOW_START);
    dataFlowStart->setContextPointer(readReq_);
    dataFlowStart->setHandle(readReq_->getHandle());
    module_->send(dataFlowStart);
}

void Read::readData()
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
    spfsOSFileReadRequest* fileRead =
        new spfsOSFileReadRequest(0, SPFS_OS_FILE_READ_REQUEST);
    Filename filename(readReq_->getHandle());
    fileRead->setContextPointer(readReq_);
    fileRead->setFilename(filename.c_str());

    // Add the file regions to the request
    vector<FileRegion> regions = layout.getRegions();
    fileRead->setOffsetArraySize(regions.size());
    fileRead->setExtentArraySize(regions.size());
    for (size_t i = 0; i < regions.size(); i++)
    {
        fileRead->setOffset(i, regions[i].offset);
        fileRead->setExtent(i, regions[i].extent);
    }

    // Send the message
    module_->send(fileRead);
}

void Read::sendFinalResponse()
{
    // Construct the final response
    spfsReadResponse* resp = new spfsReadResponse(
        0, SPFS_READ_RESPONSE);
    resp->setContextPointer(readReq_);
    resp->setByteLength(4);
    module_->send(resp);
}

void Read::finish()
{
    cerr << "Read flow completed.  Delete the read request?." << endl;
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
