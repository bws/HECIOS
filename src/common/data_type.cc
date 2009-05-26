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
#include "data_type.h"
#include <cassert>
#include <cmath>
using namespace std;

// To determine the extent for strided types is difficult due to negative
// strides, do all that work in this static helper
size_t DataType::calculateStridedExtent(size_t count, size_t blockLength,
                                        int32_t stride, size_t elementExtent)
{
    int32_t lower, upper;
    if (stride < 0)
    {
        lower = (count - 1) * stride * elementExtent;
        upper = blockLength * elementExtent;
    }
    else
    {
        lower = 0;
        upper = ((count - 1) * stride + blockLength) * elementExtent;
    }
    assert(upper >= lower);
    return upper - lower;
}

DataType::DataType(size_t extent)
    : extent_(extent),
      trueExtent_(extent)
{
    assert(extent_ > 0);
    assert(trueExtent_ >= extent_);
}

DataType::DataType(const DataType& other)
    : extent_(other.extent_),
      trueExtent_(other.trueExtent_)
{
    assert(extent_ > 0);
    assert(trueExtent_ >= extent_);
}

DataType::~DataType()
{
}

size_t DataType::getExtent() const
{
    return extent_;
}

size_t DataType::getTrueExtent() const
{
    return trueExtent_;
}

void DataType::resize(int lowerBound, size_t extent)
{
    cerr << __FILE__ << ":" << __LINE__ << ": Calling resize is dangerous!\n";
    assert(0 <= extent);
    extent = lowerBound + extent;
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
