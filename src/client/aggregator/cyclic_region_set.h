#ifndef CYCLIC_REGION_SET_H_
#define CYCLIC_REGION_SET_H_

//
// This file is part of Hecios
//
// Copyright (C) 2008 Bradley W. Settlemyer
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
