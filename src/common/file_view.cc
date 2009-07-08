//
// This file is part of Hecios
//
// Copyright (C) 2007,2008,2009 Brad Settlemyer
//
// This file is distributed WITHOUT ANY WARRANTY. See the file 'License.txt'
// for details on this and other legal matters.
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

bool FileView::operator ==(const FileView& other) const
{
    // FIXME: Fix the data type comparison to use equivalence
    return (displacement_ == other.displacement_ &&
            dataType_ == other.dataType_);
}

std::ostream& operator<<(std::ostream& ost, const FileView& fv)
{
    return ost << "FileView(disp=" << fv.getDisplacement() << ","
               << "dt=" << fv.getDataType()->getExtent() << ")";
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
