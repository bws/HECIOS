//
// This file is part of Hecios
//
// Copyright (C) 2007,2008,2009 Brad Settlemyer
//
// This file is distributed WITHOUT ANY WARRANTY. See the file 'License.txt'
// for details on this and other legal matters.
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
    extent_ = lowerBound + extent;
}

ostream& DataType::print(ostream& ost) const
{
    ost << "Not Implemented for this data type.";
    return ost;
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
