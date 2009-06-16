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
#include "data_type.h"
#include "data_type_processor.h"
#include "file_builder.h"
#include "indexed_data_type.h"
#include "mpi_proto_m.h"
#include "subarray_data_type.h"
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
    const DataType* elementType = 0;
    vector<size_t> sizes;
    vector<size_t> subSizes;
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
            elementType = subArray->getOldType();
            sizes = subArray->getSizes();
            subSizes = subArray->getSubSizes();
        }
        crs->insert(subArray);

        // Determine the amount of memory buffer to send
        bufferSize += first->getCount() * first->getDataType()->getTrueExtent();
        first++;
    }


    // TODO: Need a selection for non-contiguous types here?
    // Now perform dilation for the composition type?
    int dilationIdx = sizes.size() - 1;
    dilationSize = sizes[dilationIdx] / subSizes[dilationIdx];
    fileOffset *= dilationSize;

    // Create the new request using the new data type
    vector<spfsMPIFileRequest*> viewAwareRequests;
    FileDescriptor* viewAwareDescriptor = createDescriptor(*filename,
                                                           *crs,
                                                           elementType,
                                                           sizes);
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
                                                          const CyclicRegionSet& crs,
                                                          const DataType* elementType,
                                                          const vector<size_t>& sizes)
{
    assert(0 != crs.size());

    // The default file view will be fine here
    FileDescriptor* viewAwareDescriptor =
        FileBuilder::instance().getDescriptor(filename);

    if (1 < crs.size())
    {
        vector<size_t> blockLengths;
        vector<size_t> displacements;
        CyclicRegionSet::const_iterator first = crs.begin();
        CyclicRegionSet::const_iterator last = crs.end();
        while (first != last)
        {
            //cerr << "Cyclic regions: " << *first << endl;
            FileRegion fr = *(first++);
            blockLengths.push_back(fr.extent);
            displacements.push_back(fr.offset);
        }

        // Construct an indexed data type here
        IndexedDataType* indexed = new IndexedDataType(blockLengths,
                                                       displacements,
                                                       *elementType);
        indexed->resize(0, crs.cycleSize());

        // Set the file view
        FileView view(0, indexed);
        viewAwareDescriptor->setFileView(view);

        //cerr << "Using the aggregate indexed type" << endl;
        //for (size_t i = 0; i < sizes.size(); i++)
        //{
        //    cerr << "BlockLen: " << blockLengths[i] << " Displacements: " << displacements[i] << endl;
        //}
    }
    else if (crs.cycleSize() != crs.regionSpan() &&
             1 == crs.size())
    {
        vector<size_t> subSizes;
        vector<size_t> starts;

        // This assumes C Order, and is way too simplistic
        subSizes.push_back(sizes[0]);
        starts.push_back(0);

        // Initialize the remainder of the dimensions
        CyclicRegionSet::const_iterator iter = crs.begin();
        subSizes.push_back(iter->extent);
        starts.push_back(iter->offset);

        // Construct a new subarray type here
        SubarrayDataType* subarray = new SubarrayDataType(sizes,
                                                          subSizes,
                                                          starts,
                                                          SubarrayDataType::C_ORDER,
                                                          *elementType);

        // Set the file view
        FileView view(0, subarray);
        viewAwareDescriptor->setFileView(view);

        //cerr << "Using the new subarray jobu" << endl;
        //for (size_t i = 0; i < sizes.size(); i++)
        //{
        //    cerr << "Size: " << sizes[i] << " Subsize: " << subSizes[i] << " start: " << starts[i] << endl;
        //}
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
