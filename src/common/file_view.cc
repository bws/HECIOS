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
#include <algorithm>
#include "file_view.h"
#include "data_type.h"
using namespace std;

FileView::FileView(const FSOffset& displacement, DataType* dataType)
    : displacement_(displacement),
      dataType_(dataType)
{
}

FileView::FileView( const FileView& other)
    : displacement_(other.displacement_),
      dataType_(other.dataType_->clone())
{
}

FileView::~FileView()
{
    delete dataType_;
}

FileView& FileView::operator=(const FileView& other)
{
    // Use swap idiom to assign values safely
    FileView tmp(other);
    swap(tmp);
    return *this;
}

void FileView::swap(FileView& other)
{
    // Swap the contents of this and other piecemeal
    std::swap(displacement_, other.displacement_);
    std::swap(dataType_, other.dataType_);
}

std::size_t FileView::getRepresentationByteLength() const
{
    return 8 + dataType_->getRepresentationByteLength();
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
