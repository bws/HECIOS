#ifndef BASIC_DISTRIBUTION_H
#define BASIC_DISTRIBUTION_H

#include "file_distribution.h"

/**
 * Basic one server file distribution
 */
class BasicDistribution public FileDistribution
{
public:

    /** The size of contguous blocks within the basic distribution */
    static const int CONTGUOUS_SIZE = 65536;
    
    /** Constructor */
    BasicDistribution() : FileDistribution(1) {};

    /** Destructor */
    virtual ~BasicDistribution() {};

protected:

    /** @return the physical offset for a logical offset */
    virtual int logicalToPhysicalOffset(int logicalOffset) const {return logicalOffset;};

    /** @return the logical offset for a physical offset */
    virtual int nextMappedLogicalOffset(int logicalOffset) const {return logicalOffset; };

    /** @return the logical offset for a physical offset */
    virtual int physicalToLogicalOffset(int physicalOffset) const {return physicalOffset};

    /** @return the contiguous length forward from a physical offset */
    virtual int contiguousLength(int physicalOffset) const {return CONTGUOUS_SIZE; };

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
