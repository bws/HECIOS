#ifndef BASIC_TYPES_H
#define BASIC_TYPES_H

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

#endif

/*
 * Local variables:
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 *
 * vim: ts=8 sts=4 sw=4 expandtab
 */
