#ifndef VIEW_AWARE_ACCESS_STRATEGY_H_
#define VIEW_AWARE_ACCESS_STRATEGY_H_
//
// This file is part of Hecios
//
// Copyright (C) 2009 Brad Settlemyer
//
// This file is distributed WITHOUT ANY WARRANTY. See the file 'License.txt'
// for details on this and other legal matters.
//
#include "aggregator_access_strategy.h"
class CyclicRegionSet;
class DataType;
class Filename;
class FileDescriptor;

class ViewAwareAccessStrategy : public AggregatorAccessStrategy
{
public:
    /** Constructor */
    ViewAwareAccessStrategy();

    /** Destructor */
    ~ViewAwareAccessStrategy();

protected:
    /** Construct union of request regions */
    virtual std::vector<spfsMPIFileRequest*> performUnion(const std::set<AggregationIO>& requests);

private:
    /** Construct union of request regions */
    std::vector<spfsMPIFileRequest*> subarrayUnion(const std::set<AggregationIO>& requests,
                                                   bool isRead);

    /** @return a descriptor with a view for the cyclic region set */
    FileDescriptor* createDescriptor(const Filename& filename,
                                     const CyclicRegionSet& crs,
                                     const DataType* elementType,
                                     const std::vector<std::size_t>& sizes);
};


#endif /* VIEW_AWARE_ACCESS_STRATEGY_H_ */

/*
 * Local variables:
 *  indent-tabs-mode: nil
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 *
 * vim: ts=4 sts=4 sw=4 expandtab
 */
