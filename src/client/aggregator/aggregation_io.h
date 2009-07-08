#ifndef AGGREGATION_IO_H_
#define AGGREGATION_IO_H_
//
// This file is part of Hecios
//
// Copyright (C) 2009 Brad Settlemyer
//
// This file is distributed WITHOUT ANY WARRANTY. See the file 'License.txt'
// for details on this and other legal matters.
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
