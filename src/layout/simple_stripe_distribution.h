#ifndef SIMPLE_STRIPE_DISTRIBUTION_H
#define SIMPLE_STRIPE_DISTRIBUTION_H

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
    SimpleStripeDistribution(std::size_t numServers,
                             FSSize stripSize = DEFAULT_STRIP_SIZE);

    /** Destructor */
    virtual ~SimpleStripeDistribution() {};

protected:

    /** @return the physical offset for a logical offset */
    virtual FSOffset logicalToPhysicalOffset(FSOffset logicalOffset) const;

    /** @return the logical offset for a physical offset */
    virtual FSOffset nextMappedLogicalOffset(FSOffset logicalOffset) const;

    /** @return the logical offset for a physical offset */
    virtual FSOffset physicalToLogicalOffset(FSOffset physicalOffset) const;

    /** @return the contiguous length forward from a physical offset */
    virtual FSSize contiguousLength(FSOffset physicalOffset) const;

    /** @return the contiguous length forward from a physical offset */
    virtual FSSize logicalFileSize() const;

private:

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
