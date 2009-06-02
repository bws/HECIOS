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
#include "aggregation_io.h"
#include <cassert>
#include "file_descriptor.h"
#include "mpi_proto_m.h"
using namespace std;

AggregationIO AggregationIO::createAggregationIO(spfsMPIFileRequest* req)
{
    if (SPFS_MPI_FILE_READ_AT_REQUEST == req->kind())
    {
        return AggregationIO(dynamic_cast<spfsMPIFileReadAtRequest*>(req));
    }
    else
    {
        assert(SPFS_MPI_FILE_WRITE_AT_REQUEST == req->kind());
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
