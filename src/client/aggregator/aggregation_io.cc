//
// This file is part of Hecios
//
// Copyright (C) 2009 Brad Settlemyer
//
// This file is distributed WITHOUT ANY WARRANTY. See the file 'License.txt'
// for details on this and other legal matters.
//
#include "aggregation_io.h"
#include <cassert>
#include "file_descriptor.h"
#include "mpi_proto_m.h"
using namespace std;

AggregationIO AggregationIO::createAggregationIO(spfsMPIFileRequest* req)
{
    if (SPFS_MPI_FILE_READ_AT_REQUEST == req->getKind())
    {
        return AggregationIO(dynamic_cast<spfsMPIFileReadAtRequest*>(req));
    }
    else
    {
        assert(SPFS_MPI_FILE_WRITE_AT_REQUEST == req->getKind());
        return AggregationIO(dynamic_cast<spfsMPIFileWriteAtRequest*>(req));
    }
}

AggregationIO::AggregationIO(spfsMPIFileReadAtRequest* readAt)
    : count_(readAt->getCount()),
      dataType_(readAt->getDataType()),
      ioType_(READ),
      offset_(readAt->getOffset()),
      request_(readAt),
      view_(readAt->getFileDes()->getFileView())
{
    assert(0 != dataType_);
    assert(0 != request_);
}

AggregationIO::AggregationIO(spfsMPIFileWriteAtRequest* writeAt)
    : count_(writeAt->getCount()),
      dataType_(writeAt->getDataType()),
      ioType_(WRITE),
      offset_(writeAt->getOffset()),
      request_(writeAt),
      view_(writeAt->getFileDes()->getFileView())
{
    assert(0 != dataType_);
    assert(0 != request_);
}

AggregationIO::~AggregationIO()
{
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
