//
// This file is part of Hecios
//
// Copyright (C) 2008 bradles
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
#include "file_region_set.h"
#include <algorithm>
using namespace std;

FileRegionSet::FileRegionSet()
{
}

FileRegionSet::FileRegionSet(const FileRegionSet& other)
    : regions_(other.regions_)
{
}

FileRegionSet::~FileRegionSet()
{
}

FileRegionSet& FileRegionSet::operator=(const FileRegionSet& other)
{
    FileRegionSet temp(other);
    swap(temp);
    return * this;
}

set<FileRegion>::const_iterator FileRegionSet::begin() const
{
    return regions_.begin();
}

set<FileRegion>::const_iterator FileRegionSet::end() const
{
    return regions_.end();
}

set<FileRegion>::iterator FileRegionSet::begin()
{
    return regions_.begin();
}

set<FileRegion>::iterator FileRegionSet::end()
{
    return regions_.end();
}

size_t FileRegionSet::numBytes() const
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

size_t FileRegionSet::size() const
{
    return regions_.size();
}

size_t FileRegionSet::regionSpan() const
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

void FileRegionSet::print(ostream& ost) const
{
    ost << "FileRegionSet {";
    set<FileRegion>::const_iterator first = regions_.begin();
    set<FileRegion>::const_iterator last = regions_.end();
    while (first != last)
    {
        ost << *first;
        first++;
    }
    ost << "}";
}

void FileRegionSet::insert(const FileRegion& region)
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
        cerr << "Region extent modded: " << newRegion << endl;
        regions_.erase(itUpper++);
    }

    // Finally, insert the new region (which replaces any erased regions)
    regions_.insert(newRegion);
}

void FileRegionSet::swap(FileRegionSet& other)
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
