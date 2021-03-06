//
// This file is part of Hecios
//
// Copyright (C) 2007,2008,2009 Brad Settlemyer
//
// This file is distributed WITHOUT ANY WARRANTY. See the file 'License.txt'
// for details on this and other legal matters.
//
#include "dirty_file_region_set.h"
#include <cassert>
#include <algorithm>
#include <functional>
#include "file_region_set.h"
using namespace std;

/** Filter functor for finding the regions that are or aren't dirty */
class DirtyFileRegionDirtyFilter : public unary_function<DirtyFileRegion, bool>
{
public:
    /** Constructor */
    DirtyFileRegionDirtyFilter(bool isDirty) : isDirty_(isDirty) {};

    /** @return true if the regions dirty bit equals the filters dirty bit */
    bool operator()(const DirtyFileRegion& region)
    {
        return (region.isDirty == isDirty_);
    }

private:
    bool isDirty_;
};

DirtyFileRegion::DirtyFileRegion(FSOffset offsetV, FSSize extentV, bool isDirtyV)
    : isDirty(isDirtyV)
{
    offset = offsetV;
    extent = extentV;
}

DirtyFileRegionSet::DirtyFileRegionSet()
{
}

DirtyFileRegionSet::DirtyFileRegionSet(const DirtyFileRegionSet& other)
    : regions_(other.regions_)
{
}

DirtyFileRegionSet::DirtyFileRegionSet(const FileRegionSet& regionSet, bool isDirty)
{
    FileRegionSet::const_iterator iter = regionSet.begin();
    FileRegionSet::const_iterator end = regionSet.end();
    while (iter != end)
    {
        DirtyFileRegion dfr(iter->offset, iter->extent, isDirty);
        regions_.insert(dfr);
        ++iter;
    }
}

DirtyFileRegionSet::~DirtyFileRegionSet()
{
}

DirtyFileRegionSet& DirtyFileRegionSet::operator=(const DirtyFileRegionSet& other)
{
    DirtyFileRegionSet temp(other);
    swap(temp);
    return * this;
}

DirtyFileRegionSet::const_iterator DirtyFileRegionSet::begin() const
{
    return regions_.begin();
}

DirtyFileRegionSet::const_iterator DirtyFileRegionSet::begin(bool isDirty) const
{
    // We need the boost filtered iterator to support this method
    assert(false);
    DirtyFileRegionDirtyFilter dirtyFilter(isDirty);
    return find_if(regions_.begin(), regions_.end(), dirtyFilter);
}

DirtyFileRegionSet::const_iterator DirtyFileRegionSet::end() const
{
    return regions_.end();
}

DirtyFileRegionSet::iterator DirtyFileRegionSet::begin()
{
    return regions_.begin();
}

DirtyFileRegionSet::iterator DirtyFileRegionSet::begin(bool isDirty)
{
    // We need the boost filtered iterator to support this method
    assert(false);
    DirtyFileRegionDirtyFilter dirtyFilter(isDirty);
    return find_if(regions_.begin(), regions_.end(), dirtyFilter);
}

set<DirtyFileRegion>::iterator DirtyFileRegionSet::end()
{
    return regions_.end();
}

size_t DirtyFileRegionSet::numBytes() const
{
    size_t numBytes = 0;
    set<DirtyFileRegion>::const_iterator first = regions_.begin();
    set<DirtyFileRegion>::const_iterator last = regions_.end();
    while (first != last)
    {
        numBytes += first->extent;
        first++;
    }
    return numBytes;
}

size_t DirtyFileRegionSet::size() const
{
    return regions_.size();
}

size_t DirtyFileRegionSet::size(bool isDirty) const
{
    DirtyFileRegionDirtyFilter dirtyFilter(isDirty);
    return count_if(regions_.begin(), regions_.end(), dirtyFilter);
}

size_t DirtyFileRegionSet::regionSpan() const
{
    size_t firstByte = 0;
    size_t lastByte = 0;
    if (0 != regions_.size())
    {
        set<DirtyFileRegion>::const_iterator first = regions_.begin();
        set<DirtyFileRegion>::const_iterator last = regions_.end();
        firstByte = first->offset;
        --last;
        lastByte = last->offset + last->extent;
    }
    return (lastByte - firstByte);
}

void DirtyFileRegionSet::print(ostream& ost) const
{
    ost << "DirtyFileRegionSet {";
    set<DirtyFileRegion>::const_iterator first = regions_.begin();
    set<DirtyFileRegion>::const_iterator last = regions_.end();
    while (first != last)
    {
        ost << *first;
        first++;
    }
    ost << "}";
}

void DirtyFileRegionSet::insert(const DirtyFileRegion& region)
{
    // Create a modifiable copy of the region to be inserted
    DirtyFileRegion newRegion(region);

    // Check if this insertion extends any existing offsets before this region
    if (0 != regions_.size())
    {
        set<DirtyFileRegion>::iterator itLow = regions_.upper_bound(region);
        if (itLow != regions_.begin())
        {
            itLow--;
            // There are 4 possibilities here:
            //   1.  The regions overlap and dirty bits match, extend the region
            //   2.  The regions overlap but the new region is dirty and
            //        the existing region is clean, insert new region and
            //        adjust old region
            //   3.  The regions overlap but the old region is dirty and the
            //        new region is clean, adjust the offset of the new region
            //        and insert
            //   4.  The regions don't overlap at all, no processing
            FSOffset prevRegionEnd = itLow->offset + itLow->extent;
            if (region.offset <= prevRegionEnd)
            {
                if (region.isDirty == itLow->isDirty)
                {
                    FSOffset newRegionEnd = region.offset + region.extent;
                    FSOffset maxEnd = max(newRegionEnd, prevRegionEnd);
                    newRegion.offset = itLow->offset;
                    newRegion.extent = maxEnd - newRegion.offset;
                    regions_.erase(itLow);
                }
                else if (region.isDirty)
                {
                    // New region stays the same, just modify the old lower region
                    DirtyFileRegion lowerRegion(*itLow);
                    lowerRegion.extent = (region.offset) - lowerRegion.offset;
                    regions_.erase(itLow);
                    regions_.insert(lowerRegion);
                }
                else
                {
                    // Old lower region stays the same, just modify the new region
                    newRegion.offset = itLow->offset + itLow->extent;
                    newRegion.extent = region.extent -(newRegion.offset - region.offset);
                }
            }
        }
    }

    // Check if the new region subsumes any of the following regions
    FSOffset newRegionEnd = newRegion.offset + newRegion.extent;
    set<DirtyFileRegion>::iterator itUpper = regions_.upper_bound(region);
    set<DirtyFileRegion>::iterator last = regions_.end();
    while(itUpper != last && newRegionEnd >= itUpper->offset)
    {
        // Here we know that itUpper needs to be combined into our current
        // region, there are 3 cases:
        //   1.  The new region is the same as the upper region or
        //         the new region is dirty and extends beyond the upper region;
        //         consume the upper region entirely
        //   2.  The new region is dirty, the upper region is clean, and
        //         the new region does not extend past the upper region;
        //         truncate the new region at the dirty bound
        //   3.  The new region is clean, the upper region is dirty, and
        //         the new region does not extend past the upper region;
        //         truncate the new region at the dirty bound
        //   4.  The new region is clean, the upper region is dirty, and
        //         the new region does extend past the upper region;
        //         truncate the new region at the dirty bound, insert it,
        //         and create another new region to continue processing with
        FSOffset upperRegionEnd = itUpper->offset + itUpper->extent;
        if ((newRegion.isDirty == itUpper->isDirty) ||
            (newRegion.isDirty && (newRegionEnd > upperRegionEnd)))
        {
            // Subsume the upper region into the new region
            FSOffset nextRegionEnd = itUpper->offset + itUpper->extent;
            FSOffset maxEnd = max(newRegionEnd, nextRegionEnd);
            newRegion.extent = maxEnd - newRegion.offset;
            regions_.erase(itUpper++);
            //cerr << "Region extent modded: " << newRegion << endl;
        }
        else if (newRegion.isDirty && newRegionEnd <= upperRegionEnd)
        {
            // Erase the upper region and add it back with the new offset/extent
            regions_.erase(itUpper++);
            DirtyFileRegion newUpper(newRegionEnd,
                                     upperRegionEnd - newRegionEnd,
                                     false);
            regions_.insert(newUpper);
            break;
        }
        else if (newRegionEnd <= upperRegionEnd)
        {
            newRegion.extent = itUpper->offset - newRegion.offset;
            break;
        }
        else
        {
            // Since this is a set, we can insert without invalidating the
            // iterator
            DirtyFileRegion temp(newRegion);
            temp.extent = itUpper->offset - newRegion.offset;
            regions_.insert(temp);

            // Modify the new region's offset and extent to skip over the
            // dirty range
            newRegion.offset = itUpper->offset + itUpper->extent;
            newRegion.extent = newRegionEnd - newRegion.offset;
            ++itUpper;
        }
    }
    // Finally, insert the new region (which replaces any erased regions)
    regions_.insert(newRegion);
}

void DirtyFileRegionSet::swap(DirtyFileRegionSet& other)
{
    std::swap(this->regions_, other.regions_);
}

/*
 * Local variables:
 *  indent-tabs-mode: nil
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 *
 * vim: ts=4 sts=4 sw=4 expandtab
 */
