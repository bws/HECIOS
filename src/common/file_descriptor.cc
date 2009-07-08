//
// This file is part of Hecios
//
// Copyright (C) 2007,2008,2009 Brad Settlemyer
//
// This file is distributed WITHOUT ANY WARRANTY. See the file 'License.txt'
// for details on this and other legal matters.
//
#include "file_descriptor.h"
#include <cassert>
#include "basic_data_type.h"
#include "file_builder.h"
using namespace std;

FileDescriptor::FileDescriptor(const Filename& name,
                               const FSMetaData& metaData)
    : communicator_(SPFS_COMM_SELF),
      filename_(name),
      metaData_(metaData),
      fileView_(0, new ByteDataType()),
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

void FileDescriptor::setCommunicator(const Communicator& communicator)
{
    communicator_ = communicator;
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

bool FileDescriptor::operator==(const FileDescriptor& other) const
{
    return (filename_ == other.filename_ &&
            filePtr_ == other.filePtr_ &&
            fileView_ == other.fileView_);
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
