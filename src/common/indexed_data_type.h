#ifndef INDEXED_DATA_TYPE_H_
#define INDEXED_DATA_TYPE_H_
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
#include "data_type.h"

/**
 * A indexed file view data type (similar to MPI data types)
 */
class IndexedDataType : public DataType
{
public:
    static std::size_t calculateExtent(
        const std::vector<std::size_t>& blockLengths,
        const std::vector<std::size_t>& displacements,
        const DataType& oldDataType);

    /** Constructor */
    IndexedDataType(const std::vector<std::size_t>& blocklengths,
                    const std::vector<std::size_t>& displacements,
                    const DataType& oldType);

    /** Destructor */
    virtual ~IndexedDataType();

    /** @return a copy of this */
    virtual IndexedDataType* clone() const;

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
    IndexedDataType(const IndexedDataType& other);

    /** @return the number of element regions in the type */
    std::size_t count() const { return blockLengths_.size(); };

    /** @return The old data type */
    const DataType& oldType() const { return oldType_; };

private:
    /** Hidden assignment operator */
    IndexedDataType& operator=(const IndexedDataType& other);

    /** @return the number of bytes of data contained in this type */
    size_t getDataSize() const;

    /** The number of elements in each indexed block */
    std::vector<std::size_t> blockLengths_;

    /** The displacments for each element block measured as elements */
    std::vector<std::size_t> displacements_;

    /** The old data type to aggregate */
    const DataType& oldType_;
};

#endif /*INDEXED_DATA_TYPE_H_*/

/*
 * Local variables:
 *  indent-tabs-mode: nil
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 *
 * vim: ts=4 sts=4 sw=4 expandtab
 */
