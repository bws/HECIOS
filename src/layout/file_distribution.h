#ifndef FILE_DISTRIBUTION_H
#define FILE_DISTRIBUTION_H
//
// This file is part of Hecios
//
// Copyright (C) 2007,2008,2009 Brad Settlemyer
//
// This file is distributed WITHOUT ANY WARRANTY. See the file 'License.txt'
// for details on this and other legal matters.
//
#include <cstddef>
#include "pfs_types.h"

/**
 * Abstract File distribution.  Derived distributions will need to provide
 * implementations for all of the private virtual functions.
 */
class FileDistribution
{
public:

    /** Constructor */
    FileDistribution(std::size_t objectIdx, std::size_t numObjects);

    /** Copy Constructor */
    FileDistribution(const FileDistribution& other);

    /** Destructor */
    virtual ~FileDistribution() {};

    /** @return a pointer to a correctly copied concrete FileDistribution */
    FileDistribution* clone() const { return doClone(); };

    /** @return the object index for this node's file distribution */
    int getObjectIdx() const { return objectIdx_; };

    /** @return the number of data objects used by this distribution */
    int getNumObjects() const { return numObjects_; };

    /** Set object index */
    void setObjectIdx(std::size_t objectIdx) { objectIdx_ = objectIdx; };

    /** @return the physical offset for a logical offset */
    FSOffset logicalToPhysicalOffset(FSOffset logicalOffset) const;

    /** @return the logical offset for a physical offset */
    FSOffset nextMappedLogicalOffset(FSOffset logicalOffset) const;

    /** @return the logical offset for a physical offset */
    FSOffset physicalToLogicalOffset(FSOffset physicalOffset) const;

    /** @return the contiguous length forward from a physical offset */
    FSSize contiguousLength(FSOffset physicalOffset) const;

    /** @return the logical file size */
    FSSize logicalFileSize() const;

protected:

    /** the index number for this data object */
    std::size_t objectIdx_;

    /** the total number of data objects */
    std::size_t numObjects_;

private:

    /** @return a pointer to a correctly copied derived FileDistribution */
    virtual FileDistribution* doClone() const = 0;

    /** @return the contiguous length forward from a physical offset */
    virtual FSSize getContiguousLength(std::size_t objectIdx,
                                       FSOffset physicalOffset) const = 0;

    /** @return the logical file size */
    virtual FSSize getLogicalFileSize() const = 0;

    /** @return the logical offset for a physical offset */
    virtual FSOffset getNextMappedLogicalOffset(
        std::size_t objectIdx, FSOffset logicalOffset) const = 0;

    /** @return the physical offset for a logical offset */
    virtual FSOffset convertLogicalToPhysicalOffset(
        std::size_t objectIdx, FSOffset logicalOffset) const = 0;

    /** @return the logical offset for a physical offset */
    virtual FSOffset convertPhysicalToLogicalOffset(
        std::size_t objectIdx, FSOffset physicalOffset) const = 0;

};

#endif

/*
 * Local variables:
 *  indent-tabs-mode: nil
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 *
 * vim: ts=8 sts=4 sw=4 expandtab
 */
