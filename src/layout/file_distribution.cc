
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
