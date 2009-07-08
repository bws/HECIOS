#ifndef FILE_PAGE_H_
#define FILE_PAGE_H_
//
// This file is part of Hecios
//
// Copyright (C) 2007,2008,2009 Brad Settlemyer
//
// This file is distributed WITHOUT ANY WARRANTY. See the file 'License.txt'
// for details on this and other legal matters.
//
#include <cstddef>
#include <iostream>
#include <set>
#include "basic_types.h"

/** An identifier for a file page */
typedef std::size_t FilePageId;

/** A file page */
class FilePage
{
public:
    /** Constructor */
    FilePage(FSOffset offset, FSSize size) : offset_(offset), size_(size) {};

    /** @return the page's beginning offset */
    FSOffset offset() const { return offset_; };

    /** @return the page's size/extent */
    FSSize size() const { return size_; };

private:
    FSOffset offset_;

    FSSize size_;
};

/** @return true if lhs' offset is less than rhs' offset */
inline bool operator<(const FilePage& lhs, const FilePage& rhs)
{
    return (lhs.offset() < rhs.offset());
}

#endif /*FILE_PAGE_H_*/

/*
 * Local variables:
 *  indent-tabs-mode: nil
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 *
 * vim: ts=8 sts=4 sw=4 expandtab
 */
