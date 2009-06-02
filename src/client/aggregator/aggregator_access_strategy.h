#ifndef AGGREGATOR_ACCESS_STRATEGY_H_
#define AGGREGATOR_ACCESS_STRATEGY_H_
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
