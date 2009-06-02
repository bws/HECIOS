#ifndef AGGREGATION_IO_H_
#define AGGREGATION_IO_H_
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
#include <cstddef>
#include "basic_types.h"
#include "file_view.h"
class DataType;
class spfsMPIFileRequest;
class spfsMPIFileReadAtRequest;
class spfsMPIFileWriteAtRequest;

class AggregationIO
{
public:
    /** @return an aggregation IO for the request */
    static AggregationIO createAggregationIO(spfsMPIFileRequest* req);

    /** IOType */
    enum IOType {INVALID = 0, READ = 1, WRITE = 2};

    /** Constructor for read requests */
    AggregationIO(spfsMPIFileReadAtRequest* readAt);

    /** Constructor for write requests */
    AggregationIO(spfsMPIFileWriteAtRequest* writeAt);

    /** Destructor */
    ~AggregationIO();

    /** @return request buffer count */
    std::size_t getCount() const { return count_; };

    /** @return request buffer data type */
    const DataType* getDataType() const { return dataType_; };

    /** @return the IO type */
    IOType getIOType() const { return ioType_; };

    /** @return request offset */
    FSOffset getOffset() const { return offset_; };

    /** @return request */
    spfsMPIFileRequest* getRequest() const { return request_; };

    /** @return request file view */
    const FileView* getView() const { return &view_; };

private:
    std::size_t count_;

    const DataType* dataType_;

    IOType ioType_;

    FSOffset offset_;

    spfsMPIFileRequest* request_;

    const FileView view_;
};

inline bool operator<(const AggregationIO& lhs, const AggregationIO& rhs)
{
    return (lhs.getRequest() < rhs.getRequest());
}

#endif /* AGGREGATION_IO_H_ */

/*
 * Local variables:
 *  indent-tabs-mode: nil
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 *
 * vim: ts=4 sts=4 sw=4 expandtab
 */
