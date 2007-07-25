#ifndef FILE_DISTRIBUTION_H
#define FILE_DISTRIBUTION_H

#include <cstddef>
#include "pfs_types.h"

/**
 * Abstract File distribution
 */
class FileDistribution
{
public:

    /** Constructor */
    FileDistribution(std::size_t numDataObjects)
        : numDataObjects_(numDataObjects) {};

    /** Destructor */
    virtual ~FileDistribution() {};

    /** @return the number of data objects used by this distribution */
    int getNumObjects() const { return numDataObjects_; };

protected:

    /** @return the physical offset for a logical offset */
    virtual FSOffset logicalToPhysicalOffset(FSOffset logicalOffset) const = 0;

    /** @return the logical offset for a physical offset */
    virtual FSOffset nextMappedLogicalOffset(FSOffset logicalOffset) const = 0;

    /** @return the logical offset for a physical offset */
    virtual FSOffset physicalToLogicalOffset(FSOffset physicalOffset) const =0;

    /** @return the contiguous length forward from a physical offset */
    virtual FSSize contiguousLength(FSOffset physicalOffset) const = 0;

    /** @return the logical file size */
    virtual FSSize logicalFileSize() const = 0;

    std::size_t numDataObjects_;
    
};

#endif

/*
 * Local variables:
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 *
 * vim: ts=8 sts=4 sw=4 expandtab
 */
