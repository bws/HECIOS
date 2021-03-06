#ifndef DIRTY_FILE_REGION_SET_H_
#define DIRTY_FILE_REGION_SET_H_
//
// This file is part of Hecios
//
// Copyright (C) 2007,2008,2009 Brad Settlemyer
//
// This file is distributed WITHOUT ANY WARRANTY. See the file 'License.txt'
// for details on this and other legal matters.
//
#include <iostream>
#include <set>
#include "basic_types.h"
class FileRegionSet;

/** A file region that includes a dirty bit */
class DirtyFileRegion : public FileRegion
{
public:
    DirtyFileRegion(FSOffset offset, FSSize extent, bool isDirty);
    bool isDirty;
};

/** A set of file regions that seperates clean and dirty regions */
class DirtyFileRegionSet
{
public:
    /** Convenience typedef for iterator type */
    typedef std::set<DirtyFileRegion>::const_iterator const_iterator;

    /** Convenience typedef for iterator type */
    typedef std::set<DirtyFileRegion>::iterator iterator;

    /** Default constructor */
    DirtyFileRegionSet();

    /** Copy constructor hidden */
    DirtyFileRegionSet(const DirtyFileRegionSet& other);

    /** Constructor new dirty region set from plain regions and a dirty bit */
    DirtyFileRegionSet(const FileRegionSet& regionSet, bool isDirty);

    /** Destructor */
   ~DirtyFileRegionSet();

    /** Assignment operator hidden */
    DirtyFileRegionSet& operator=(const DirtyFileRegionSet& other);

    /** @return an iterator to the beginning of the set */
    const_iterator begin() const;

    /**
     * @deprecated
     * @return an iterator to the beginning of the set of regions
     *   marked dirty
     */
    const_iterator begin(bool isDirty) const;

    /** @return an iterator to the end of the set */
    const_iterator end() const;

    /** @return an iterator to the beginning of the set */
    iterator begin();

    /**
     * @deprecated
     * @return an iterator to the beginning of the set of regions
     *   marked dirty
     */
    iterator begin(bool isDirty);

    /** @return an iterator to the end of the set */
    iterator end();

    /** @return the number of bytes in all of the regions */
    std::size_t numBytes() const;

    /** @return the distance between the first and last region byte */
    std::size_t regionSpan() const;

    /** @return the number of regions */
    std::size_t size() const;

    /** @return the number of regions marked isDirty*/
    std::size_t size(bool isDirty) const;

    /** Add this region set into the stream */
    virtual void print(std::ostream& ost) const;

    /**
     * Insert the region into the set, merging with abutting regions
     * as necessary.
     */
    void insert(const DirtyFileRegion& region);

private:
    /** Swap the contents of this and other */
    void swap(DirtyFileRegionSet& other);

    std::set<DirtyFileRegion> regions_;
};

inline std::ostream& operator<<(std::ostream& ost, const DirtyFileRegion& region)
{
    ost << "[" << region.offset << "," << region.extent << ","
        << region.isDirty << "]";
    return ost;
}

inline bool operator<(const DirtyFileRegion& lhs, const DirtyFileRegion& rhs)
{
    bool lhsIsLess = true;
    if (lhs.offset != rhs.offset)
    {
        lhsIsLess = (lhs.offset < rhs.offset);
    }
    else if (lhs.extent != rhs.extent)
    {
        lhsIsLess = (lhs.extent < rhs.extent);
    }
    return lhsIsLess;
}

inline bool operator==(const DirtyFileRegion& lhs, const DirtyFileRegion& rhs)
{
    bool isEqual = (lhs.offset == rhs.offset) &&
                   (lhs.extent == rhs.extent) &&
                   (lhs.isDirty == rhs.isDirty);
    return isEqual;
}

inline std::ostream& operator<<(std::ostream& ost, const DirtyFileRegionSet& regionSet)
{
    regionSet.print(ost);
    return ost;
}

#endif /* DIRTY_FILE_REGION_SET_H_ */

/*
 * Local variables:
 *  indent-tabs-mode: nil
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 *
 * vim: ts=4 sts=4 sw=4 expandtab
 */
