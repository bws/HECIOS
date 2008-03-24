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
#include "get_attr.h"
#include <cassert>
#include "file_builder.h"
#include "filename.h"
#include "fs_server.h"
#include "os_proto_m.h"
#include "pvfs_proto_m.h"
using namespace std;

GetAttr::GetAttr(FSServer* module, spfsGetAttrRequest* getAttrReq)
    : module_(module),
      getAttrReq_(getAttrReq)
{
}

void GetAttr::handleServerMessage(cMessage* msg)
{
    // Restore the existing state for this request
    cFSM currentState = getAttrReq_->getState();

    // Server lookup states
    enum {
        INIT = 0,
        READ_ATTR = FSM_Steady(1),
        FINISH = FSM_Steady(2),
    };

    FSM_Switch(currentState)
    {
        case FSM_Exit(INIT):
        {
            module_->recordGetAttr();
            FSM_Goto(currentState, READ_ATTR);
            break;
        }
        case FSM_Enter(READ_ATTR):
        {
            assert(0 != dynamic_cast<spfsGetAttrRequest*>(msg));
            enterReadAttr();
            break;
        }
        case FSM_Exit(READ_ATTR):
        {
            FSM_Goto(currentState, FINISH);
            break;
        }
        case FSM_Enter(FINISH):
        {
            assert(0 != dynamic_cast<spfsOSFileReadResponse*>(msg));
            module_->recordGetAttrDiskDelay(msg);
            enterFinish();
            break;
        }
    }

    // Store current state
    getAttrReq_->setState(currentState);
}

void GetAttr::enterReadAttr()
{
    // Convert the handle into a local file name
    Filename filename(getAttrReq_->getHandle());

    // Create the file write request
    spfsOSFileReadRequest* fileRead = new spfsOSFileReadRequest();
    fileRead->setFilename(filename.c_str());
    fileRead->setOffsetArraySize(1);
    fileRead->setExtentArraySize(1);
    fileRead->setOffset(0, 0);
    fileRead->setExtent(0, module_->getDefaultAttrSize());
    fileRead->setContextPointer(getAttrReq_);
    
    // Send the write request
    module_->send(fileRead);
}

void GetAttr::enterFinish()
{
    // Calculate the size of the response
    size_t responseSize = 0;
    if (SPFS_METADATA_OBJECT == getAttrReq_->getObjectType())
    {
        // Lookup the number of handles for this file
        FSHandle metaHandle = getAttrReq_->getHandle();
        size_t numHandles =
            FileBuilder::instance().getNumDataObjects(metaHandle);
        size_t handleArrayBytes = 8 * numHandles;
        responseSize = FSServer::METADATA_ATTRIBUTES_BYTE_SIZE +
            handleArrayBytes;
    }
    else if (SPFS_DIRECTORY_OBJECT == getAttrReq_->getObjectType())
    {
        responseSize = FSServer::DIRECTORY_ATTRIBUTES_BYTE_SIZE;
    }
    else
    {
        responseSize = FSServer::DATAFILE_ATTRIBUTES_BYTE_SIZE;
    }
    
    // Send the final response
    spfsGetAttrResponse* resp = new spfsGetAttrResponse(
        0, SPFS_GET_ATTR_RESPONSE);
    resp->setContextPointer(getAttrReq_);
    resp->setByteLength(responseSize);

    // Add processing delay for processing
    module_->sendDelayed(resp, FSServer::getAttrProcessingDelay());
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
