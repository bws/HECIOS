//
// This file is part of Hecios
//
// Copyright (C) 2007 Brad Settlemyer
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

#include "file_distribution.h"
#include <cassert>
using namespace std;

FileDistribution::FileDistribution(size_t objectIdx, size_t numObjects)
    : objectIdx_(objectIdx),
      numObjects_(numObjects)
{
    assert(objectIdx_ < numObjects_);
}

FileDistribution::FileDistribution(const FileDistribution& other)
    : objectIdx_(other.objectIdx_),
      numObjects_(other.numObjects_)
{
    assert(objectIdx_ < numObjects_);
}

FSOffset FileDistribution::logicalToPhysicalOffset(
    FSOffset logicalOffset) const
{
    return convertLogicalToPhysicalOffset(objectIdx_, logicalOffset);
}

FSOffset FileDistribution::physicalToLogicalOffset(
    FSOffset logicalOffset) const
{
    return convertPhysicalToLogicalOffset(objectIdx_, logicalOffset);
}

FSOffset FileDistribution::nextMappedLogicalOffset(
    FSOffset logicalOffset) const
{
    return getNextMappedLogicalOffset(objectIdx_, logicalOffset);
}

FSSize FileDistribution::contiguousLength(FSOffset physicalOffset) const
{
    return getContiguousLength(objectIdx_, physicalOffset);
}

FSSize FileDistribution::logicalFileSize() const
{
    return getLogicalFileSize();
}

/*
 * Local variables:
 *  mode: c
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 *
 * vim: ft=c ts=8 sts=4 sw=4 expandtab
 */
