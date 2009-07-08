#ifndef VECTOR_DATA_TYPE_H
#define VECTOR_DATA_TYPE_H
//
// This file is part of Hecios
//
// Copyright (C) 2007,2008,2009 Brad Settlemyer
//
// This file is distributed WITHOUT ANY WARRANTY. See the file 'License.txt'
// for details on this and other legal matters.
//
#include <cstddef>
#include "data_type.h"

/**
 * A vector file view data type (similar to MPI data types)
 */
class VectorDataType : public DataType
{
public:
    /** Default constructor */
    VectorDataType(std::size_t count,
                   std::size_t blockLength,
                   int stride,
                   const DataType& oldType);

    /** Destructor */
    virtual ~VectorDataType();

    /** @return a copy of this */
    virtual VectorDataType* clone() const;

    /** @return the number of bytes required to represent this data type */
    std::size_t getRepresentationByteLength() const;

   /**
     * @return the *data* regions for numBytes of data in this DataType.
     * Note that the DataType may contain empty holes, thus leading to
     * numBytes of data corresponding to a much larger DataType extent.
     */
    virtual std::vector<FileRegion> getRegionsByBytes(
        const FSOffset& byteOffset, std::size_t numBytes) const;

protected:
    /** Copy constructor for use by clone */
    VectorDataType(const VectorDataType& other);

private:
    /** Hidden assignment operator */
    VectorDataType& operator=(const VectorDataType& other);

    /** The number of blocks */
    std::size_t count_;

    /** Number of elements in each block */
    std::size_t blockLength_;

    /** Spacing between the start of each block, measured as the number of
        oldTypes (may be negative) */
    int stride_;

    /** The old type to aggregate */
    const DataType& oldType_;
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
