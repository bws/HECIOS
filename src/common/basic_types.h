#ifndef BASIC_TYPES_H
#define BASIC_TYPES_H
//
// This file is part of Hecios
//
// Copyright (C) 2007 Walt Ligon
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

#include <stdint.h>
#include <iostream>

/** File system block data type */
typedef int64_t FSBlock;

/** File system handle data type */
typedef uint64_t FSHandle;

/** File system file offset data type */
typedef uint64_t FSOffset;

/** File system file region length data type */
typedef uint64_t FSSize;

/** Logical Block Address data type */
typedef int64_t LogicalBlockAddress;

/** A contiguous region of a file, i.e. an offset and an extent */
struct FileRegion
{
    FSOffset offset;
    FSSize extent;
};
    
/** A contguous handle range beginning at first and ending at last */
struct HandleRange
{
    FSHandle first;
    FSHandle last;
};

/** Equality operation for HandleRanges */
inline bool operator==(const HandleRange& lhs, const HandleRange& rhs)
{
    std::cerr << "LHS: " << lhs.first << " " <<lhs.last
              << " RHS: " << rhs.first << " " << rhs.last << std::endl;
    return (lhs.first == rhs.first && lhs.last == rhs.last);
}

/**
 * Less than operation for HandleRanges
 *
 * Only provided so that HandleRanges can be inserted into maps,
 * Note:  Assumes that HandleRanges do not overlap!!
 */
inline bool operator<(const HandleRange& lhs, const HandleRange& rhs)
{
    return (lhs.first < rhs.first && lhs.last < rhs.last);
}

/**
 * Absolute value function for 64bit values
 */
inline int64_t abs64(int64_t val)
{
    return (val >= 0) ? val : -val;
}

#endif

/*
 * Local variables:
 *  indent-tabs-mode: nil
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 *
 * vim: ts=8 sts=4 sw=4 expandtab
 */
