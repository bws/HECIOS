#ifndef DATA_SIEVING_ACCESS_STRATEGY_H_
#define DATA_SIEVING_ACCESS_STRATEGY_H_
//
// This file is part of Hecios
//
// Copyright (C) 2009 Brad Settlemyer
//
// This file is distributed WITHOUT ANY WARRANTY. See the file 'License.txt'
// for details on this and other legal matters.
//
#include "aggregator_access_strategy.h"
#include "aggregation_io.h"
class spfsMPIFileRequest;
class FileDescriptor;

/** */
class DataSievingAccessStrategy : public AggregatorAccessStrategy
{
public:
    /** Constructor */
    DataSievingAccessStrategy();

    /** Destructor */
    ~DataSievingAccessStrategy();

protected:
    /** Construct union of request regions */
    std::vector<spfsMPIFileRequest*> performUnion(const std::set<AggregationIO>& requests);

private:
    /** @return Descriptor describing the data sieved range */
    FileDescriptor* createDescriptor(const std::set<AggregationIO>& requests,
                                     FSOffset offset,
                                     FSSize extent);
};


#endif /* DATA_SIEVING_ACCESS_STRATEGY_H_ */

/*
 * Local variables:
 *  indent-tabs-mode: nil
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 *
 * vim: ts=4 sts=4 sw=4 expandtab
 */
