#ifndef DATA_TYPE_LAYOUT
#define DATA_TYPE_LAYOUT
//
// This file is part of Hecios
//
// Copyright (C) 2007,2008,2009 Brad Settlemyer
//
// This file is distributed WITHOUT ANY WARRANTY. See the file 'License.txt'
// for details on this and other legal matters.
//
#include <cstddef>
#include <vector>
#include "basic_types.h"
#include "data_type.h"

/**
 * File layout for an I/O request's data type, count, and extent
 */
class DataTypeLayout
{
public:
    /** Default constructor */
    DataTypeLayout();

    /** Construct DataTypeLayout */
    DataTypeLayout(DataType* dataType);

    /** Construct a layout for a single contiguous region */
    DataTypeLayout(const FSOffset& offset, const FSSize& extent);

    /** Construct a layout from corresponding lists of offsets and extents */
    DataTypeLayout(const std::vector<FSOffset>& offsets,
                   const std::vector<FSSize>& extents);

    /** Add the layout regions for the offset and extent */
    void addRegion(const FSOffset& offset, const FSSize& extent);

    /** Add the layout regions from corresponding lists of offsets and
        extents */
    void addRegions(const std::vector<FSOffset>& offsets,
                    const std::vector<FSSize>& extents);

    /** @return all the file regions for this request data type */
    std::vector<FileRegion> getRegions() const;

    /** @return the idx'th file region for this request data type */
    FileRegion getRegion(std::size_t idx) const;

    /** Return the total length of this request data layout */
    FSSize getLength() const;

    /** @return the file regions in the request layout from
        [byteOffset, byteOffset + byteLength] */
    std::vector<FileRegion> getSubRegions(const FSOffset& byteOffset,
                                          const FSSize& byteLength) const;

private:
    /** The list of contiguous regions described in this layout */
    std::vector<FileRegion> fileRegions_;

};

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
