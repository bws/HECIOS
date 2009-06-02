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
#include "data_sieving_access_strategy.h"
#include <algorithm>
#include <cassert>
#include "basic_data_type.h"
#include "data_type.h"
#include "data_type_processor.h"
#include "file_builder.h"
#include "file_view.h"
#include "mpi_proto_m.h"
using namespace std;

static ByteDataType byteType;

DataSievingAccessStrategy::DataSievingAccessStrategy()
{
}

DataSievingAccessStrategy::~DataSievingAccessStrategy()
{
}

vector<spfsMPIFileRequest*>
DataSievingAccessStrategy::performUnion(const set<AggregationIO>& requests)
{
    FSOffset reqBegin, reqEnd;
    set<AggregationIO>::const_iterator first = requests.begin();
    set<AggregationIO>::const_iterator last = requests.end();
    while (first != last)
    {
        assert(AggregationIO::READ == first->getIOType());
        FSSize bufferSize = first->getCount() * first->getDataType()->getTrueExtent();
        vector<FileRegion> regions = DataTypeProcessor::locateFileRegions(first->getOffset(),
                                                                          bufferSize,
                                                                          *(first->getView()));
        // Initialize offsets
        if (first == requests.begin())
        {
            reqBegin = regions[0].offset;
            reqEnd = regions[0].offset + regions[0].extent;
        }

        // Search for the maximum inclusive range
        for (size_t i = 0; i < regions.size(); i++)
        {
            reqBegin = min(reqBegin, regions[i].offset);
            reqEnd = max(reqEnd, FSOffset(regions[i].offset + regions[i].extent));
        }

        //cerr << "Request off: " << first->getOffset() << "BufSize: " << bufferSize
        //     << "Begin: " << reqBegin << " ext: " << reqEnd - reqBegin << endl;
        first++;
    }

    // Create the new request using the maximum inclusive range
    vector<spfsMPIFileRequest*> sievingRequests;
    FSSize aggExtent = reqEnd - reqBegin;
    FileDescriptor* aggFd = createDescriptor(requests, reqBegin, aggExtent);

    spfsMPIFileReadAtRequest* aggRead =
        new spfsMPIFileReadAtRequest("Agg Read", SPFS_MPI_FILE_READ_AT_REQUEST);
    aggRead->setCount(aggExtent);
    aggRead->setDataType(&byteType);
    aggRead->setFileDes(aggFd);
    aggRead->setOffset(reqBegin);
    sievingRequests.push_back(aggRead);
    return sievingRequests;
}

FileDescriptor* DataSievingAccessStrategy::createDescriptor(const set<AggregationIO>& requests,
                                                            FSOffset offset,
                                                            FSSize extent)
{
    //cerr << __FILE__ << ":" << __LINE__ << ":"
    //     << "Data sieving request: " << offset << " " << extent << endl;
    AggregationIO aggIO = *(requests.begin());
    spfsMPIFileRequest* request = aggIO.getRequest();
    FileDescriptor* origDescriptor = request->getFileDes();

    // The default file view will be fine here
    FileDescriptor* dataSievingDescriptor =
        FileBuilder::instance().getDescriptor(origDescriptor->getFilename());
    return dataSievingDescriptor;
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
