//
// This file is part of Hecios
//
// Copyright (C) 2007 Walt Ligon
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
#include <cassert>
#include "simple_stripe_distribution.h"
using namespace std;

SimpleStripeDistribution::SimpleStripeDistribution(size_t serverIdx,
                                                   size_t numServers,
                                                   FSSize stripSize)
    : FileDistribution(serverIdx, numServers),
      stripSize_(stripSize)
{
}

SimpleStripeDistribution::SimpleStripeDistribution(
    const SimpleStripeDistribution& other )
    : FileDistribution(other),
      stripSize_(other.stripSize_)
{
}

SimpleStripeDistribution* SimpleStripeDistribution::doClone() const
{
    return new SimpleStripeDistribution(*this);
}

FSOffset SimpleStripeDistribution::convertLogicalToPhysicalOffset(
    size_t objectIdx, FSOffset logicalOffset) const
{
    // Determine the number of complete stripes
    int64_t completeStripes = logicalOffset / (stripSize_ * numObjects_);
    int64_t physicalOffset = completeStripes * stripSize_;

    // Adjust physical offset if we are not at beginning of the current strip
    int64_t leftOver = logicalOffset - (completeStripes * stripSize_ * numObjects_);
    int64_t stripBegin = objectIdx * stripSize_;
    if (leftOver >= stripBegin)
    {
        int64_t stripEnd = (objectIdx + 1) * stripSize_;
        if (leftOver < stripEnd)
        {
            physicalOffset += leftOver - (objectIdx * stripSize_);
        }
        else
        {
            physicalOffset += stripSize_;
        }
    }
    return physicalOffset;
}

FSOffset SimpleStripeDistribution::getNextMappedLogicalOffset(
    size_t objectIdx, FSOffset logicalOffset) const
{
    int64_t serverStartingOffset = objectIdx * stripSize_;
    int64_t stripeSize = numObjects_ * stripSize_;
    int64_t diff = (logicalOffset - serverStartingOffset) % stripeSize;

    if (diff < 0)
    {
        return serverStartingOffset;
    }
    else if (FSSize(diff) >= stripSize_)
    {
        return logicalOffset + (stripeSize - diff);
    }
    else
    {
        return logicalOffset;
    }
}

FSOffset SimpleStripeDistribution::convertPhysicalToLogicalOffset(
    size_t objectIdx, FSOffset physicalOffset) const
{
    int64_t stripsDiv = physicalOffset / stripSize_;
    int64_t stripsMod = physicalOffset % stripSize_;

    return (stripsDiv * stripSize_ * numObjects_) +
        (stripSize_ * objectIdx) + stripsMod;
}

FSSize SimpleStripeDistribution::getContiguousLength(
    size_t objectIdx, FSOffset physicalOffset) const
{
    return stripSize_ - (physicalOffset % stripSize_);
}

FSSize SimpleStripeDistribution::getLogicalFileSize() const
{
    assert(0 == 1);
    return 1;
}
