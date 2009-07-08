#ifndef HINDEXED_DATA_TYPE_H_
#define HINDEXED_DATA_TYPE_H_
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
#include "struct_data_type.h"
#include "data_type.h"

/**
 * A Hindexed file view data type (similar to MPI data types)
 */
class HindexedDataType : public StructDataType
{
public:
    static std::size_t calculateExtent(
        const std::vector<std::size_t>& blockLengths,
        const std::vector<std::size_t>& displacements,
        const DataType& oldDataType);

    /** Constructor */
    HindexedDataType(const std::vector<std::size_t>& blocklengths,
                     const std::vector<std::size_t>& displacements,
                     const DataType& oldType);

    /** Destructor */
    virtual ~HindexedDataType();

    /** @return a copy of this */
    virtual HindexedDataType* clone() const;

    /** @return the number of bytes required to represent this data type */
    std::size_t getRepresentationByteLength() const;

protected:
    /** Copy constructor for use by clone */
    HindexedDataType(const HindexedDataType& other);

private:
    /** Hidden assignment operator */
    HindexedDataType& operator=(const HindexedDataType& other);
};

#endif /*HINDEXED_DATA_TYPE_H_*/

/*
 * Local variables:
 *  indent-tabs-mode: nil
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 *
 * vim: ts=4 sts=4 sw=4 expandtab
 */
