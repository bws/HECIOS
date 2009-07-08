#ifndef BASIC_DISTRIBUTION_H
#define BASIC_DISTRIBUTION_H
//
// This file is part of Hecios
//
// Copyright (C) 2007,2008,2009 Brad Settlemyer
//
// This file is distributed WITHOUT ANY WARRANTY. See the file 'License.txt'
// for details on this and other legal matters.
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
