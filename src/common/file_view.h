#ifndef FILE_VIEW_H
#define FILE_VIEW_H
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
#include "basic_types.h"
#include <iosfwd>
#include <vector>
class DataType;

/**
 * A file view
 */
class FileView
{
public:
    /**
     * Constructor
     *
     * displacement - arbitrary offset from beginning of file in bytes
     * dataType - description of file regions to access (takes ownership)
     */
    FileView(const FSOffset& displacement, DataType* dataType);

    /** Copy constructor */
    FileView(const FileView& other);

    /** Destructor */
    ~FileView();

    /** Assignment operator */
    FileView& operator=(const FileView& other);

    /** @return the displacment in bytes */
    FSOffset getDisplacement() const { return displacement_; };

    /** @return this views data type */
    const DataType* getDataType() const { return dataType_; };

    /** @return the number of bytes required to represent this data type */
    std::size_t getRepresentationByteLength() const;

    /** @return true if the view is equivalent */
    bool operator==(const FileView& other) const;

private:
    /** Swap the contents of this and other */
    void swap(FileView& other);

    FSOffset displacement_;

    DataType* dataType_;
};

/** @return Output stream containg the file view */
std::ostream& operator<<(std::ostream& ost, const FileView& fv);

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
