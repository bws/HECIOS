//
// This file is part of Hecios
//
// Copyright (C) 2009 Brad Settlemyer
//
// This file is distributed WITHOUT ANY WARRANTY. See the file 'License.txt'
// for details on this and other legal matters.
//
#include "aggregator_access_strategy.h"
using namespace std;

AggregatorAccessStrategy::AggregatorAccessStrategy()
{
}

AggregatorAccessStrategy::~AggregatorAccessStrategy()
{
}

vector<spfsMPIFileRequest*>
AggregatorAccessStrategy::joinRequests(const set<AggregationIO>& requests)
{
    return performUnion(requests);
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
