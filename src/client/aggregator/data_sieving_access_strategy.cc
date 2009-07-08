//
// This file is part of Hecios
//
// Copyright (C) 2009 Brad Settlemyer
//
// This file is distributed WITHOUT ANY WARRANTY. See the file 'License.txt'
// for details on this and other legal matters.
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
