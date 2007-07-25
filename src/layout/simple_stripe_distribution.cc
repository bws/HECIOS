
#include "simple_stripe_distribution.h"
using namespace std;

SimpleStripeDistribution::SimpleStripeDistribution(
    size_t numServers,
    FSSize stripSize)
    : FileDistribution(numServers),
      stripSize_(stripSize)
{
}

FSOffset SimpleStripeDistribution::logicalToPhysicalOffset(FSOffset logOffset) const
{
    return logOffset;
}

FSOffset SimpleStripeDistribution::nextMappedLogicalOffset(FSOffset logicalOffset) const
{
    return logicalOffset;
}

FSOffset SimpleStripeDistribution::physicalToLogicalOffset(FSOffset physOffset) const
{
    return physOffset;
}

FSSize SimpleStripeDistribution::contiguousLength(FSOffset physicalOffset) const
{
    return stripSize_;
}

FSSize SimpleStripeDistribution::logicalFileSize() const
{
    return 1;
}
