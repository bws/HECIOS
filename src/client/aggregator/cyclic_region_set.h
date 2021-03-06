#ifndef CYCLIC_REGION_SET_H_
#define CYCLIC_REGION_SET_H_
//
// This file is part of Hecios
//
// Copyright (C) 2009 Brad Settlemyer
//
// This file is distributed WITHOUT ANY WARRANTY. See the file 'License.txt'
// for details on this and other legal matters.
//
#include <iostream>
#include <set>
#include "basic_types.h"
class SubarrayDataType;
class VectorDataType;

class CyclicRegionSet
{
public:
    /** Convenience typedef for iterator type */
    typedef std::set<FileRegion>::const_iterator const_iterator;

    /** Convenience typedef for iterator type */
    typedef std::set<FileRegion>::iterator iterator;

    /** Default constructor */
    CyclicRegionSet(std::size_t cycleSize);

    /** Copy constructor hidden */
    CyclicRegionSet(const CyclicRegionSet& other);

    /** Destructor */
    ~CyclicRegionSet();

    /** Assignment operator hidden */
    CyclicRegionSet& operator=(const CyclicRegionSet& other);

    /** @return an iterator to the beginning of the set */
    const_iterator begin() const;

    /** @return an iterator to the end of the set */
    const_iterator end() const;

    /** @return an iterator to the beginning of the set */
    iterator begin();

    /** @return an iterator to the end of the set */
    iterator end();

    /** @return the number of bytes in all of the regions */
    std::size_t cycleSize() const { return cycleSize_; };

    /** @return the number of bytes in all of the regions */
    std::size_t numBytes() const;

    /** @return the distance between the first and last region byte */
    std::size_t regionSpan() const;

    /** @return the number of regions */
    std::size_t size() const;

    /** Add this region set into the stream */
    virtual void print(std::ostream& ost) const;

    /**
     * Insert the region into the set, merging with abutting regions
     * as necessary.
     */
    void insert(const FileRegion& region);

    /**
     * Insert the subarray regions, merging with abutting regions
     * as necessary.
     */
    void insert(const SubarrayDataType* subarray);

    /** Perform region dilation*/
    void dilate(const std::size_t scalar);

    /** Perform region filtering */
    void selection();

private:
    /** Swap the contents of this and other */
    void swap(CyclicRegionSet& other);

    std::size_t cycleSize_;

    std::set<FileRegion> regions_;
};

inline std::ostream& operator<<(std::ostream& ost, const CyclicRegionSet& regionSet)
{
    regionSet.print(ost);
    return ost;
}

#endif /* CYCLIC_REGION_SET_H_ */

/*
 * Local variables:
 *  indent-tabs-mode: nil
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 *
 * vim: ts=4 sts=4 sw=4 expandtab
 */
