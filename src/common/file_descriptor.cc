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
#include "file_descriptor.h"
#include <cassert>
#include "basic_data_type.h"
#include "file_builder.h"
using namespace std;

FileDescriptor::FileDescriptor(const Filename& name,
                               const FSMetaData& metaData)
    : filename_(name),
      metaData_(metaData),
      fileView_(0, new BasicDataType(BasicDataType::MPI_BYTE_WIDTH)),
      filePtr_(0)
{
    // Set the root directory's handle
    Filename root = name.getSegment(0);
    FSMetaData* rootMeta = FileBuilder::instance().getMetaData(root);
    assert(0 != rootMeta);
    parentHandles_.push_back(rootMeta->handle);
}

FileDescriptor::~FileDescriptor()
{
}

size_t FileDescriptor::getNumParentHandles() const
{
    return parentHandles_.size();
}

FSHandle FileDescriptor::getParentHandle(size_t segmentIdx) const
{
    assert(segmentIdx < parentHandles_.size());
    return parentHandles_[segmentIdx]; 
}

void FileDescriptor::setFileView(const FileView& fileView)
{
    fileView_ = fileView;
}

void FileDescriptor::setFilePointer(FSOffset filePtr)
{
    filePtr_ = filePtr;
    assert(0 <= filePtr_);
}

void FileDescriptor::moveFilePointer(FSOffset inc)
{
    filePtr_ += inc;
    assert(0 <= filePtr_);
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
