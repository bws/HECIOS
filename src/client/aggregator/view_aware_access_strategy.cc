//
// This file is part of Hecios
//
// Copyright (C) 2009 Bradley W. Settlemyer
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
#include "view_aware_access_strategy.h"
#include <cassert>
#include <memory>
#include "basic_data_type.h"
#include "contiguous_data_type.h"
#include "cyclic_region_set.h"
#include "data_type_processor.h"
#include "file_builder.h"
#include "subarray_data_type.h"
#include "mpi_proto_m.h"
using namespace std;

static ByteDataType byteType;

ViewAwareAccessStrategy::ViewAwareAccessStrategy()
{
}

ViewAwareAccessStrategy::~ViewAwareAccessStrategy()
{
}

vector<spfsMPIFileRequest*>
ViewAwareAccessStrategy::performUnion(const set<AggregationIO>& requests)
{
    assert(!requests.empty());

    // Determine if this is a read or write
    spfsMPIFileRequest* req = requests.begin()->getRequest();
    bool isRead = true;
    if (0 != dynamic_cast<spfsMPIFileWriteAtRequest*>(req))
    {
        isRead = false;
    }

    return subarrayUnion(requests, isRead);
}

vector<spfsMPIFileRequest*>
ViewAwareAccessStrategy::subarrayUnion(const set<AggregationIO>& requests,
                                       bool isRead)
{
    Filename* filename = 0;
    CyclicRegionSet* crs = 0;
    size_t bufferSize = 0;
    size_t fileOffset;
    size_t dilationSize;
    set<AggregationIO>::const_iterator first = requests.begin();
    set<AggregationIO>::const_iterator last = requests.end();
    while (first != last)
    {
        const FileView* view = first->getView();
        const DataType* dType = view->getDataType();
        const SubarrayDataType* subArray = dynamic_cast<const SubarrayDataType*>(dType);

        // Initialize the cyclic region set if necessary
        if (0 == crs)
        {
            // Set the cycle to the contiguous dimension length
            crs = new CyclicRegionSet(subArray->getArrayContiguousCount());
            filename = new Filename(first->getRequest()->getFileDes()->getFilename());
            fileOffset = first->getOffset();
            dilationSize = subArray->getOldType()->getTrueExtent();
        }
        crs->insert(subArray);

        // Determine the amount of memory buffer to send
        bufferSize += first->getCount() * first->getDataType()->getTrueExtent();
        first++;
    }

    // Now perform dilation for the composition type
    crs->dilate(dilationSize);

    // Todo: Need a selection for non-contiguous types here

    // Create the new request using the new data type
    vector<spfsMPIFileRequest*> viewAwareRequests;
    FileDescriptor* viewAwareDescriptor = createDescriptor(*filename, *crs);
    if (isRead)
    {
        spfsMPIFileReadAtRequest* aggRead =
            new spfsMPIFileReadAtRequest("ViewAware Read", SPFS_MPI_FILE_READ_AT_REQUEST);
        aggRead->setCount(bufferSize);
        aggRead->setDataType(&byteType);
        aggRead->setFileDes(viewAwareDescriptor);
        aggRead->setOffset(fileOffset);
        viewAwareRequests.push_back(aggRead);
    }
    else
    {
        spfsMPIFileWriteAtRequest* aggWrite =
            new spfsMPIFileWriteAtRequest("ViewAwareWrite", SPFS_MPI_FILE_WRITE_AT_REQUEST);
        aggWrite->setCount(bufferSize);
        aggWrite->setDataType(&byteType);
        aggWrite->setFileDes(viewAwareDescriptor);
        aggWrite->setOffset(fileOffset);
        viewAwareRequests.push_back(aggWrite);
    }

    // Perform cleanup
    delete crs;
    delete filename;

    return viewAwareRequests;
}

FileDescriptor* ViewAwareAccessStrategy::createDescriptor(const Filename& filename,
                                                          const CyclicRegionSet& crs)
{
    // The default file view will be fine here
    FileDescriptor* viewAwareDescriptor =
        FileBuilder::instance().getDescriptor(filename);

    if (1 < crs.size())
    {
        // Construct an indexed data type here
        //IndexedDataType* indexed = new IndexedDataType();
    }
    else if (crs.cycleSize() != crs.regionSpan())
    {
        // Construct a new subarray type here
        //SubarrayDataType* subarray = new SubarrayDataType();
    }
    return viewAwareDescriptor;
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
