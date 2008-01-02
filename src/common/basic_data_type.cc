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
#include "basic_data_type.h"
using namespace std;

BasicDataType::BasicDataType(size_t basicTypeBytes)
    : DataType(basicTypeBytes)
{
}

BasicDataType::BasicDataType(const BasicDataType& other)
    : DataType(other)
{
}

BasicDataType::~BasicDataType()
{
}

BasicDataType* BasicDataType::clone() const
{
    return new BasicDataType(*this);
}

size_t BasicDataType::getRepresentationByteLength() const
{
    // 4 bytes should be enough
    return 4;
}

vector<FileRegion> BasicDataType::getRegionsByBytes(const FSOffset& byteOffset,
                                                    size_t numBytes) const
{
    FileRegion fr = {byteOffset, numBytes};
    vector<FileRegion> v(1, fr);
    return v;
}

vector<FileRegion> BasicDataType::getRegionsByCount(const FSOffset& byteOffset,
                                                    size_t count) const
{
    return getRegionsByBytes(byteOffset, count * getExtent());
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
