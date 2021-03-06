#ifndef BASIC_TYPES_H
#define BASIC_TYPES_H
//
// This file is part of Hecios
//
// Copyright (C) 2007,2008,2009 Brad Settlemyer
//
// This file is distributed WITHOUT ANY WARRANTY. See the file 'License.txt'
// for details on this and other legal matters.
//
#include <stdint.h>
#include <iostream>

/** File system block data type */
typedef int64_t FSBlock;

/** File system handle data type */
typedef uint64_t FSHandle;

/** File system file offset data type */
typedef int64_t FSOffset;

/** File system file region length data type */
typedef uint64_t FSSize;

/** Logical Block Address data type */
typedef int64_t LogicalBlockAddress;

/** BMI Connection Id */
typedef uint64_t ConnectionId;

/** A contiguous region of a file, i.e. an offset and an extent */
struct FileRegion
{
    FSOffset offset;
    FSSize extent;
};

/** Equality operator for File Regions */
inline bool operator==(const FileRegion& lhs, const FileRegion& rhs)
{
    return (lhs.offset == rhs.offset && lhs.extent == rhs.extent);
}

/** Less than operator for File Regions */
inline bool operator<(const FileRegion& lhs, const FileRegion& rhs)
{
    if (lhs.offset != rhs.offset)
    {
        return (lhs.offset < rhs.offset);
    }
    else
    {
        return (lhs.extent < rhs.extent);
    }
}

/** Add the file region to the stream */
inline std::ostream& operator<<(std::ostream& ost, const FileRegion& fr)
{
    ost << "[" << fr.offset << "," << fr.extent << "]";
    return ost;
}

/** A contiguous handle range beginning at first and ending at last */
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
