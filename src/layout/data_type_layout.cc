//
// This file is part of Hecios
//
// Copyright (C) 2007,2008,2009 Brad Settlemyer
//
// This file is distributed WITHOUT ANY WARRANTY. See the file 'License.txt'
// for details on this and other legal matters.
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
    //cerr << __FILE__ << ":" << __LINE__ << ":"
    //     << byteOffset << " " << byteLength << " " << getLength() << endl;
    //for(size_t i = 0; i < fileRegions_.size(); i++)
    //{
    //    cerr << "Off: " << fileRegions_[i].offset << " "
    //         << "Ext: " << fileRegions_[i].extent << endl;
    //}
    assert(0 < fileRegions_.size());
    assert(FSSize(byteOffset) < getLength());
    assert((byteOffset + byteLength) <= getLength());

    vector<FileRegion> subRegions;

    // Find the first region the sub region is contained in
    size_t regionIdx = 0;
    FSSize processedBytes = fileRegions_[0].extent;
    while (FSSize(byteOffset) >= processedBytes)
    {
        regionIdx++;
        processedBytes += fileRegions_[regionIdx].extent;
    }

    // Extract the first sub region
    FSOffset firstOffset = fileRegions_[regionIdx].offset +
        byteOffset - (processedBytes - fileRegions_[regionIdx].extent);
    FSSize firstExtent = min(fileRegions_[regionIdx].extent, byteLength);
    FileRegion firstRegion = {firstOffset, firstExtent};
    subRegions.push_back(firstRegion);

    // Extract any remaining sub regions
    regionIdx++;
    FSSize assignedBytes = firstExtent;
    while (assignedBytes < byteLength)
    {
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
