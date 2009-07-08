//
// This file is part of Hecios
//
// Copyright (C) 2009 Brad Settlemyer
//
// This file is distributed WITHOUT ANY WARRANTY. See the file 'License.txt'
// for details on this and other legal matters.
//
#include "cyclic_region_set.h"
#include <algorithm>
#include "subarray_data_type.h"
#include "vector_data_type.h"
using namespace std;

CyclicRegionSet::CyclicRegionSet(std::size_t cycleSize)
    : cycleSize_(cycleSize)
{
}

CyclicRegionSet::CyclicRegionSet(const CyclicRegionSet& other)
    : cycleSize_(other.cycleSize_),
      regions_(other.regions_)
{
}

CyclicRegionSet::~CyclicRegionSet()
{
}

CyclicRegionSet& CyclicRegionSet::operator=(const CyclicRegionSet& other)
{
    CyclicRegionSet temp(other);
    swap(temp);
    return * this;
}

set<FileRegion>::const_iterator CyclicRegionSet::begin() const
{
    return regions_.begin();
}

set<FileRegion>::const_iterator CyclicRegionSet::end() const
{
    return regions_.end();
}

set<FileRegion>::iterator CyclicRegionSet::begin()
{
    return regions_.begin();
}

set<FileRegion>::iterator CyclicRegionSet::end()
{
    return regions_.end();
}

size_t CyclicRegionSet::numBytes() const
{
    size_t numBytes = 0;
    set<FileRegion>::const_iterator first = regions_.begin();
    set<FileRegion>::const_iterator last = regions_.end();
    while (first != last)
    {
        numBytes += first->extent;
        first++;
    }
    return numBytes;
}

size_t CyclicRegionSet::size() const
{
    return regions_.size();
}

size_t CyclicRegionSet::regionSpan() const
{
    size_t firstByte = 0;
    size_t lastByte = 0;
    if (0 != regions_.size())
    {
        set<FileRegion>::const_iterator first = regions_.begin();
        set<FileRegion>::const_iterator last = regions_.end();
        firstByte = first->offset;
        --last;
        lastByte = last->offset + last->extent;
    }
    return (lastByte - firstByte);
}

void CyclicRegionSet::print(ostream& ost) const
{
    ost << "CyclicRegionSet {";
    set<FileRegion>::const_iterator first = regions_.begin();
    set<FileRegion>::const_iterator last = regions_.end();
    while (first != last)
    {
        ost << *first;
        first++;
    }
    ost << "}";
}

void CyclicRegionSet::insert(const FileRegion& region)
{
    // Create a modifiable copy of the region to be inserted
    FileRegion newRegion(region);

    // Check if this insertion extends any existing offsets before this region
    if (0 != regions_.size())
    {
        set<FileRegion>::iterator itLow = regions_.upper_bound(region);
        if (itLow != regions_.begin())
        {
            itLow--;
            FSOffset prevRegionEnd = itLow->offset + itLow->extent;
            if (region.offset <= prevRegionEnd)
            {
                FSOffset newRegionEnd = region.offset + region.extent;
                FSOffset maxEnd = max(newRegionEnd, prevRegionEnd);
                newRegion.offset = itLow->offset;
                newRegion.extent = maxEnd - newRegion.offset;
                regions_.erase(itLow);
            }
        }
    }

    // Check if the new region subsumes any of the following regions
    FSOffset newRegionEnd = newRegion.offset + newRegion.extent;
    set<FileRegion>::iterator itUpper = regions_.upper_bound(region);
    set<FileRegion>::iterator last = regions_.end();
    while(itUpper != last && newRegionEnd >= itUpper->offset)
    {
        FSOffset nextRegionEnd = itUpper->offset + itUpper->extent;
        FSOffset maxEnd = max(newRegionEnd, nextRegionEnd);
        newRegion.extent = maxEnd - newRegion.offset;
        regions_.erase(itUpper++);
    }

    // Finally, insert the new region (which replaces any erased regions)
    regions_.insert(newRegion);
}

void CyclicRegionSet::insert(const SubarrayDataType* subarray)
{
    if (cycleSize_ != subarray->getArrayContiguousCount())
    {
        cerr << __FILE__ << ":" << __LINE__ << ":"
             << "ERROR: Unmatching cycle sizes: cycle: "
             << cycleSize_ << " Subarray: " << subarray->getTrueExtent()
             << endl;
    }
    size_t elementSize = subarray->getOldType()->getExtent();
    size_t offset = subarray->getArrayMemoryLocation(0) / elementSize;
    size_t extent = subarray->getSubarrayContiguousCount();
    FileRegion fr = {offset, extent};
    insert(fr);
}

void CyclicRegionSet::dilate(size_t scalar)
{
    set<FileRegion> dilatedSet;

    set<FileRegion>::iterator first = regions_.begin();
    set<FileRegion>::iterator last = regions_.end();
    while (first != last)
    {
        FSOffset dilatedOffset = first->offset * scalar;
        FSSize dilatedExtent = first->extent * scalar;
        FileRegion fr = {dilatedOffset, dilatedExtent};
        dilatedSet.insert(fr);
        first++;
    }

    // Let the dilated set replace this set
    regions_ = dilatedSet;
    cycleSize_ *= scalar;
}

void CyclicRegionSet::selection()
{

}

void CyclicRegionSet::swap(CyclicRegionSet& other)
{
    std::swap(this->cycleSize_, other.cycleSize_);
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
