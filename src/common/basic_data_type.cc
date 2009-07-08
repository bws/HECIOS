//
// This file is part of Hecios
//
// Copyright (C) 2007,2008,2009 Brad Settlemyer
//
// This file is distributed WITHOUT ANY WARRANTY. See the file 'License.txt'
// for details on this and other legal matters.
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
