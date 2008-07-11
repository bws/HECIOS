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
//#include "basic_data_type.h"

template<std::size_t widthInBytes>
BasicDataType<widthInBytes>::BasicDataType()
    : DataType(widthInBytes)
{
}

template<std::size_t widthInBytes>
BasicDataType<widthInBytes>::BasicDataType(const BasicDataType<widthInBytes>& other)
    : DataType(other)
{
}

template<std::size_t widthInBytes>
BasicDataType<widthInBytes>::~BasicDataType()
{
}

template<std::size_t widthInBytes>
BasicDataType<widthInBytes>* BasicDataType<widthInBytes>::clone() const
{
    return new BasicDataType(*this);
}

template<std::size_t widthInBytes>
size_t BasicDataType<widthInBytes>::getRepresentationByteLength() const
{
    // 4 bytes should be enough
    return 4;
}

template<std::size_t widthInBytes>
std::vector<FileRegion> BasicDataType<widthInBytes>::getRegionsByBytes(
    const FSOffset& byteOffset,
    size_t numBytes) const
{
    FileRegion fr = {byteOffset, numBytes};
    std::vector<FileRegion> v(1, fr);
    return v;
}

template<std::size_t widthInBytes>
std::vector<FileRegion> BasicDataType<widthInBytes>::getRegionsByCount(
    const FSOffset& byteOffset,
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
