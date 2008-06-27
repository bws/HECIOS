//
// This file is part of Hecios
//
// Copyright (C) 2008 bradles
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
#include "struct_data_type.h"
using namespace std;

StructDataType::StructDataType(vector<size_t>, vector<size_t>, vector<DataType*> types)
    : DataType(0),
        types_(types)
{

}

StructDataType::StructDataType(const StructDataType& other)
    : DataType(other)
{

}

StructDataType::~StructDataType()
{

}

StructDataType* StructDataType::clone() const
{
    return new StructDataType(*this);
}

size_t StructDataType::getRepresentationByteLength() const
{
    size_t length = 0;
    for (size_t i = 0; i < types_.size(); i++)
    {
        length += 4 + 4 + types_[i]->getRepresentationByteLength();
    }
    return length;
}

vector<FileRegion> StructDataType::getRegionsByBytes(const FSOffset& byteOffset,
                                                     size_t bytes) const
{
    vector<FileRegion> regions;
    return regions;
}

vector<FileRegion> StructDataType::getRegionsByCount(const FSOffset& byteOffset,
                                                     size_t count) const
{
    vector<FileRegion> regions;
    return regions;
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
