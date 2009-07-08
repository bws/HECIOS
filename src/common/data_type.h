#ifndef DATA_TYPE_H
#define DATA_TYPE_H
//
// This file is part of Hecios
//
// Copyright (C) 2007,2008,2009 Brad Settlemyer
//
// This file is distributed WITHOUT ANY WARRANTY. See the file 'License.txt'
// for details on this and other legal matters.
//
#include <cstddef>
#include <iostream>
#include <vector>
#include "basic_types.h"

/**
 * An abstract file view data type (analogous to MPI data types)
 */
class DataType
{
public:
    /** Static helper fuction for computing the extent of strided data types */
    static std::size_t calculateStridedExtent(std::size_t count,
                                              std::size_t blockLength,
                                              int32_t stride,
                                              std::size_t elementExtent);

    /** Construct a data type with extent extent */
    DataType(std::size_t extent);

    /** Destructor */
    virtual ~DataType();

    /** @return a copy of the correct concrete derived class */
    virtual DataType* clone() const = 0;

    /** @return the size of a single element of this data type */
    std::size_t getExtent() const;

    /** @return the true size of a single element of this data type */
    std::size_t getTrueExtent() const;

    /** Sets the extent to lowerBound + extent */
    void resize(int lowerBound, std::size_t extent);

    /** @return the number of bytes required to represent this data type */
    virtual std::size_t getRepresentationByteLength() const = 0;

    /**
     * @return the *data* regions for numBytes of data in this DataType.
     * Note that the DataType may contain empty holes, thus leading to
     * numBytes of data corresponding to a much larger DataType extent.
     */
    virtual std::vector<FileRegion> getRegionsByBytes(
        const FSOffset& byteOffset, std::size_t numBytes) const = 0;

    virtual std::ostream& print(std::ostream& ost) const;

protected:
    /** Copy constructor may be called by clone */
    DataType(const DataType& other);

private:
    /** Hidden assignment operator */
    DataType& operator=(const DataType& other);

    /** The data type extent, may be resized to a false value */
    std::size_t extent_;

    /** The true data type extent */
    std::size_t trueExtent_;
};

/** Add the data type to output stream */
inline std::ostream& operator<<(std::ostream& ost, const DataType& dataType)
{
    return dataType.print(ost);
}

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
