#ifndef DATA_SIEVING_ACCESS_STRATEGY_H_
#define DATA_SIEVING_ACCESS_STRATEGY_H_
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
