#ifndef FILE_DISTRIBUTION_H
#define FILE_DISTRIBUTION_H
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
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 *
 * vim: ts=8 sts=4 sw=4 expandtab
 */
