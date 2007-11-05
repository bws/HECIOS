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
#include "data_flow.h"
#include <cassert>
#include <numeric>
#include <omnetpp.h>
#include "data_type_processor.h"
#include "file_distribution.h"
#include "filename.h"
#include "pvfs_proto_m.h"
using namespace std;

DataFlow::DataFlow(const spfsDataFlowStart& flowStart,
                   size_t numBuffers,
                   FSSize bufferSize)
    : originatingMessage_(&flowStart),
      numBuffers_(numBuffers),
      bufferSize_(bufferSize),
      mode_(INVALID),
      uniqueId_(simulation.getUniqueNumber()),
      flowSize_(0),
      clientTransferTotal_(0),
      storageTransferTotal_(0)
{
    assert(0 != flowStart.contextPointer());
    
    FSOffset offset = 0;
    FSSize dataType = 0;
    FSSize count = 0;
    FileDistribution* dist = 0;
    cMessage* msg = static_cast<cMessage*>(flowStart.contextPointer());
    if (msg->kind() == SPFS_READ_REQUEST)
    {
        mode_ = READ;
        spfsReadRequest* read = static_cast<spfsReadRequest*>(msg);
        offset = read->getOffset();
        dataType = read->getDataType();
        count = read->getCount();
        dist = read->getDist();
    }
    else
    {
        assert(SPFS_WRITE_REQUEST == msg->kind());
        mode_ = WRITE;
        spfsWriteRequest* write = static_cast<spfsWriteRequest*>(msg);
        offset = write->getOffset();
        dataType = write->getDataType();
        count = write->getCount();
        dist = write->getDist();
    }

    // Create the data type layout
    DataTypeProcessor::createFileLayoutForServer(offset, dataType, count,
                                                 *dist, 10000000, layout_);
    flowSize_ = layout_.getLength();
}

DataFlow::~DataFlow()
{
}

spfsDataFlowStart* DataFlow::getOriginatingMessage() const
{
    return const_cast<spfsDataFlowStart*>(originatingMessage_);
}

void DataFlow::initialize()
{
    // Spawn the first operations
    for (size_t i = 0; i < numBuffers_; i++)
    {
        assert(INVALID != mode_);
        if (READ == mode_)
        {
            // Request the data from storage
            pullDataFromStorage(bufferSize_);
        }
        else
        {
            // Request the data from the client
            pullDataFromClient(bufferSize_);
        }
    }
}

bool DataFlow::isComplete() const
{
    return (flowSize_ == clientTransferTotal_) &&
        (flowSize_ == storageTransferTotal_);
}

bool DataFlow::isClientComplete() const
{
    return (flowSize_ == clientTransferTotal_);
}

bool DataFlow::isStorageComplete() const
{
    return (flowSize_ == storageTransferTotal_);
}

void DataFlow::handleServerMessage(cMessage* msg)
{
    assert(0 != msg);
    processDataFlowMessage(msg);
}

void DataFlow::addClientProgress(FSSize dataTransferred)
{
    clientTransferTotal_ += dataTransferred;
}

void DataFlow::addStorageProgress(FSSize dataTransferred)
{
    storageTransferTotal_ += dataTransferred;
}

/*
 * Local variables:
 *  indent-tabs-mode: nil
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 *
 * vim: ts=4 sts=4 sw=4 expandtab foldmethod=marker
 */
