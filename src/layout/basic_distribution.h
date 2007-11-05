#ifndef BASIC_DISTRIBUTION_H
#define BASIC_DISTRIBUTION_H
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

/**
 * Basic one server file distribution
 */
class BasicDistribution : public FileDistribution
{
public:

    /** The size of contguous blocks within the basic distribution */
    static const int CONTIGUOUS_SIZE = 65536;
    
    /** Constructor */
    BasicDistribution() : FileDistribution(0, 1) {};

    /** Destructor */
    virtual ~BasicDistribution() {};

private:
    /** @return a pointer to a correctly copied derived FileDistribution */
    virtual FileDistribution* doClone() const { return new BasicDistribution(); };

    /** @return the physical offset for a logical offset */
    virtual FSOffset convertLogicalToPhysicalOffset(size_t objectIdx,
                                               FSOffset logicalOffset) const
        { return logicalOffset; };

    /** @return the logical offset for a physical offset */
    virtual FSOffset getNextMappedLogicalOffset(size_t objectIdx,
                                           FSOffset logicalOffset) const
        { return logicalOffset; };

    /** @return the logical offset for a physical offset */
    virtual FSOffset convertPhysicalToLogicalOffset(
        size_t objectIdx, FSOffset physicalOffset) const
        { return physicalOffset; };

    /** @return the contiguous length forward from a physical offset */
    virtual FSSize getContiguousLength(size_t objectIdx,
                                    FSOffset physicalOffset) const
        { return CONTIGUOUS_SIZE; };

    /** @return the logical file size */
    virtual FSSize getLogicalFileSize() const { return 0; };

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
