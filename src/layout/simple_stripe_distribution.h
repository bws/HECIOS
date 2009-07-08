#ifndef SIMPLE_STRIPE_DISTRIBUTION_H
#define SIMPLE_STRIPE_DISTRIBUTION_H
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
 * Simple striping file distribution
 */
class SimpleStripeDistribution : public FileDistribution
{
public:

    /** Default size of contiguous strips for each server */
    static const FSSize DEFAULT_STRIP_SIZE = 65536;

    /** Constructor */
    SimpleStripeDistribution(std::size_t serverIdx,
                             std::size_t numServers,
                             FSSize stripSize = DEFAULT_STRIP_SIZE);

    /** Copy Constructor */
    SimpleStripeDistribution(const SimpleStripeDistribution& other);

    /** Destructor */
    virtual ~SimpleStripeDistribution() {};

private:

    /** @return a cloned copy of this SimpleStripeDistribution*/
    virtual SimpleStripeDistribution* doClone() const;

    /** @return the contiguous length forward from a physical offset */
    virtual FSSize getContiguousLength(std::size_t objectIdx,
                                       FSOffset physicalOffset) const;

    /** @return the logical file size */
    virtual FSSize getLogicalFileSize() const;

    /** @return the logical offset for a physical offset */
    virtual FSOffset getNextMappedLogicalOffset(
        std::size_t objectIdx,
        FSOffset logicalOffset) const;

    /** @return the physical offset for a logical offset */
    virtual FSOffset convertLogicalToPhysicalOffset(
        std::size_t objectIdx,
        FSOffset logicalOffset) const;

    /** @return the logical offset for a physical offset */
    virtual FSOffset convertPhysicalToLogicalOffset(
        std::size_t objectIdx,
        FSOffset physicalOffset) const;

    FSSize stripSize_;
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
