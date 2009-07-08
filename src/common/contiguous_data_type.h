#ifndef CONTIGUOUS_DATA_TYPE_H
#define CONTIGUOUS_DATA_TYPE_H
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
 * A contiguous file view data type (similar to MPI data types)
 */
class ContiguousDataType : public DataType
{
public:
    /** Default constructor */
    ContiguousDataType(std::size_t count, const DataType& oldType);

    /** Destructor */
    virtual ~ContiguousDataType();

    /** @return a cloned copy of this */
    virtual ContiguousDataType* clone() const;

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
    /** Copy constructor */
    ContiguousDataType(const ContiguousDataType& other);

private:
    /** Hidden assignment operator */
    ContiguousDataType& operator=(const ContiguousDataType& other);

    /** The count of contiguous old types */
    std::size_t count_;

    /** The old type to aggregate */
    DataType* oldType_;
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
