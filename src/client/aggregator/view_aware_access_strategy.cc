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
#include "view_aware_access_strategy.h"
#include "data_type_processor.h"
#include "mpi_proto_m.h"
using namespace std;

ViewAwareAccessStrategy::ViewAwareAccessStrategy()
{
}

ViewAwareAccessStrategy::~ViewAwareAccessStrategy()
{
}

vector<spfsMPIFileRequest*>
ViewAwareAccessStrategy::performUnion(const set<AggregationIO>& requests)
{
    return subarrayUnion(requests);
}

vector<spfsMPIFileRequest*>
ViewAwareAccessStrategy::subarrayUnion(const set<AggregationIO>& requests)
{
    set<AggregationIO>::const_iterator first = requests.begin();
    set<AggregationIO>::const_iterator last = requests.end();
    while (first != last)
    {
        FSSize bufferSize = first->getCount() * first->getDataType()->getTrueExtent();
        vector<FileRegion> regions = DataTypeProcessor::locateFileRegions(first->getOffset(),
                                                                          bufferSize,
                                                                          *(first->getView()));

        //FileDescriptor* fd = (*first)->getFileDes();

        first++;
    }

    vector<spfsMPIFileRequest*> subarrayUnion;
    return subarrayUnion;
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
