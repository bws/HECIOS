//
// This file is part of Hecios
//
// Copyright (C) 2008 Bradley W. Settlemyer
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
#include <cassert>
#include "block_indexed_data_type.h"
using namespace std;

BlockIndexedDataType::BlockIndexedDataType(const size_t& blockLength,
                                           const vector<size_t>& displacements,
                                           const DataType& oldDataType)
    : IndexedDataType(vector<size_t>(displacements.size(), blockLength),
                      displacements,
                      oldDataType)
{
}

BlockIndexedDataType::BlockIndexedDataType(const BlockIndexedDataType& other)
    : IndexedDataType(other)
{
}

BlockIndexedDataType::~BlockIndexedDataType()
{
}

BlockIndexedDataType* BlockIndexedDataType::clone() const
{
    return new BlockIndexedDataType(*this);
}

size_t BlockIndexedDataType::getRepresentationByteLength() const
{
    return IndexedDataType::getRepresentationByteLength();
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
