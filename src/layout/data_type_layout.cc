//
// This file is part of Hecios
//
// Copyright (C) 2007 Brad Settlemyer
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
#include "data_type_layout.h"
#include <cassert>
using namespace std;

DataTypeLayout::DataTypeLayout()
{
}

DataTypeLayout::DataTypeLayout(const FSOffset& offset, const FSSize& extent)
{
    addRegion(offset, extent);
}

DataTypeLayout::DataTypeLayout(const vector<FSOffset>& offsets,
                               const vector<FSSize>& extents)
{
    addRegions(offsets, extents);
}

void DataTypeLayout::addRegion(const FSOffset& offset, const FSSize& extent)
{
    FileRegion fr = {offset, extent};
    fileRegions_.push_back(fr);
}

void DataTypeLayout::addRegions(const vector<FSOffset>& offsets,
                                const vector<FSSize>& extents)
{
    assert(offsets.size() == extents.size());
    size_t numRegions = offsets.size();
    fileRegions_.reserve(numRegions);
    for (size_t i = 0; i < numRegions; i++)
    {
        addRegion(offsets[i], extents[i]);
    }
}

vector<FileRegion> DataTypeLayout::getRegions() const
{
    return fileRegions_;
}

FileRegion DataTypeLayout::getRegion(std::size_t idx) const
{
    assert(idx < fileRegions_.size());
    return fileRegions_[idx];
}

FSSize DataTypeLayout::getLength() const
{
    FSSize length = 0;
    for (size_t i = 0; i < fileRegions_.size(); i++)
    {
        length += fileRegions_[i].extent;
    }
    return length;
}

vector<FileRegion> DataTypeLayout::getSubRegions(const FSOffset& byteOffset,
                                                 const FSSize& byteLength) const
{
    assert(0 < fileRegions_.size());
    assert(byteOffset < getLength());
    assert((byteOffset + byteLength) < getLength());

    vector<FileRegion> subRegions;
    
    // Find the first region the sub region is contained in
    size_t regionIdx = 0;
    FSSize processedBytes = 0;
    for ( ; byteOffset > processedBytes; regionIdx++)
    {
        processedBytes += fileRegions_[regionIdx].extent;
    }

    // Extract the sub regions
    FSSize assignedBytes = 0;
    while (assignedBytes < byteLength)
    {
        //cerr << "Adding region: " << regionIdx << endl;
        FSOffset offset = fileRegions_[regionIdx].offset;
        FSSize length = min(fileRegions_[regionIdx].extent,
                            byteLength - assignedBytes);
        FileRegion fr = {offset, length};
        subRegions.push_back(fr);

        // Bookkeeping
        assignedBytes += length;
        regionIdx++;
    }
    assert(assignedBytes == byteLength);
    
    return subRegions;
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
