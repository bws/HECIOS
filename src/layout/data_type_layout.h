#ifndef DATA_TYPE_LAYOUT
#define DATA_TYPE_LAYOUT
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
#include <cstddef>
#include <vector>
#include "basic_types.h"

/**
 * File layout for an I/O request's data type, count, and extent
 */
class DataTypeLayout
{
public:
    /** Default constructor */
    DataTypeLayout();
    
    /** Construct a layout for a single contiguous region */
    DataTypeLayout(const FSOffset& offset, const FSSize& extent);

    /** Construct a layout from corresponding lists of offsets and extents */
    DataTypeLayout(const std::vector<FSOffset>& offsets,
                   const std::vector<FSSize>& extents);

    /** Add the layout regions for the offset and extent */
    void addRegion(const FSOffset& offset, const FSSize& extent);

    /** Add the layout regions from corresponding lists of offsets and
        extents */
    void addRegions(const std::vector<FSOffset>& offsets,
                    const std::vector<FSSize>& extents);

    /** @return all the file regions for this request data type */
    std::vector<FileRegion> getRegions() const;

    /** @return the idx'th file region for this request data type */
    FileRegion getRegion(std::size_t idx) const;

    /** Return the total length of this request data layout */
    FSSize getLength() const;
    
    /** @return the file regions in the request layout from
        [byteOffset, byteOffset + byteLength] */
    std::vector<FileRegion> getSubRegions(const FSOffset& byteOffset,
                                          const FSSize& byteLength) const;
    
private:
    /** The list of contiguous regions described in this layout */
    std::vector<FileRegion> fileRegions_;
};

#endif

/*
 * Local variables:
 *  indent-tabs-mode: nil
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 *
 * vim: ts=4 sts=4 sw=4 expandtab
 */
