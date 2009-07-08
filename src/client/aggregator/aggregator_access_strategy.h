#ifndef AGGREGATOR_ACCESS_STRATEGY_H_
#define AGGREGATOR_ACCESS_STRATEGY_H_
//
// This file is part of Hecios
//
// Copyright (C) 2009 Brad Settlemyer
//
// This file is distributed WITHOUT ANY WARRANTY. See the file 'License.txt'
// for details on this and other legal matters.
//
#include <set>
#include <vector>
#include "aggregation_io.h"
class spfsMPIFileRequest;

/** */
class AggregatorAccessStrategy
{
public:
    /** Constructor */
    AggregatorAccessStrategy();

    /** Destructor */
    virtual ~AggregatorAccessStrategy() = 0;

    /** */
    std::vector<spfsMPIFileRequest*> joinRequests(const std::set<AggregationIO>& requests);

protected:
    /** */
    virtual std::vector<spfsMPIFileRequest*> performUnion(const std::set<AggregationIO>& requests) = 0;
};

#endif /* AGGREGATOR_ACCESS_STRATEGY_H_ */

/*
 * Local variables:
 *  indent-tabs-mode: nil
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 *
 * vim: ts=4 sts=4 sw=4 expandtab
 */
