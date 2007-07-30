
#include "simple_stripe_distribution.h"
using namespace std;

SimpleStripeDistribution::SimpleStripeDistribution(size_t serverIdx,
                                                   size_t numServers,
                                                   FSSize stripSize)
    : FileDistribution(serverIdx, numServers),
      stripSize_(stripSize)
{
}

FSOffset SimpleStripeDistribution::convertLogicalToPhysicalOffset(
    size_t objectIdx, FSOffset logicalOffset) const
{
    // Determine the number of complete stripes
    int completeStripes = logicalOffset / (stripSize_ * numObjects_);
    FSOffset physicalOffset = completeStripes * stripSize_;

    // Adjust physical offset if we are not at beginning of the current strip
    int leftOver =
        logicalOffset - (completeStripes * stripSize_ * numObjects_);

    if (leftOver >= (objectIdx * stripSize_))
    {
        if (leftOver < ((objectIdx + 1) * stripSize_))
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
    FSOffset serverStartingOffset = objectIdx * stripSize_;
    FSSize stripeSize = numObjects_ * stripSize_;
    FSOffset diff = (logicalOffset - serverStartingOffset) % stripeSize;

    if (diff < 0)
    {
        return serverStartingOffset;
    }
    else if (diff >= stripSize_)
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
    FSSize stripsDiv = physicalOffset / stripSize_;
    FSSize stripsMod = physicalOffset % stripSize_;

    FSOffset acc = (stripsDiv - 1) * stripSize_ * objectIdx;
    if (0 != stripsMod)
    {
        acc += stripSize_ * numObjects_;
        acc += stripSize_ * objectIdx_;
        acc += stripsMod;
    }
    else
    {
        acc += stripSize_ * (objectIdx + 1);
    }
    
    return acc;
}

FSSize SimpleStripeDistribution::getContiguousLength(
    size_t objectIdx, FSOffset physicalOffset) const
{
    return stripSize_ - (physicalOffset % stripSize_);
}

FSSize SimpleStripeDistribution::getLogicalFileSize() const
{
    return 1;
}
