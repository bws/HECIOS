#ifndef FILE_PAGE_H_
#define FILE_PAGE_H_
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
#include <cstddef>
#include "basic_types.h"

/** An identifier for a file page */
typedef std::size_t FilePageId;

/** A file page */
class FilePage
{
public:
    FilePage(FSOffset offset, FSSize size) : offset_(offset), size_(size) {};

    FSOffset offset() const { return offset_; };

    FSSize size() const { return size_; };

private:
    FSOffset offset_;

    FSSize size_;
};

bool operator<(const FilePage& lhs, const FilePage& rhs)
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
