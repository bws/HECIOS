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
#include "hindexed_data_type.h"
using namespace std;

HindexedDataType::HindexedDataType(const vector<size_t>& blockLengths,
                                   const vector<size_t>& displacements,
                                   const DataType& oldDataType)
    : StructDataType(blockLengths,
                     displacements,
                     vector<const DataType*>(blockLengths.size(), &oldDataType))
{
}

HindexedDataType::HindexedDataType(const HindexedDataType& other)
  : StructDataType(other)
{
}

HindexedDataType::~HindexedDataType()
{

}

HindexedDataType* HindexedDataType::clone() const
{
    return new HindexedDataType(*this);
}

size_t HindexedDataType::getRepresentationByteLength() const
{
    size_t length = 4 + (4 + 4) * count() +
        oldType(0)->getRepresentationByteLength();
    return length;
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
