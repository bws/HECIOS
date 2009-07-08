#ifndef FILE_VIEW_H
#define FILE_VIEW_H
//
// This file is part of Hecios
//
// Copyright (C) 2007,2008,2009 Brad Settlemyer
//
// This file is distributed WITHOUT ANY WARRANTY. See the file 'License.txt'
// for details on this and other legal matters.
//
#include "basic_types.h"
#include <iosfwd>
#include <vector>
class DataType;

/**
 * A file view
 */
class FileView
{
public:
    /**
     * Constructor
     *
     * displacement - arbitrary offset from beginning of file in bytes
     * dataType - description of file regions to access (takes ownership)
     */
    FileView(const FSOffset& displacement, DataType* dataType);

    /** Copy constructor */
    FileView(const FileView& other);

    /** Destructor */
    ~FileView();

    /** Assignment operator */
    FileView& operator=(const FileView& other);

    /** @return the displacment in bytes */
    FSOffset getDisplacement() const { return displacement_; };

    /** @return this views data type */
    const DataType* getDataType() const { return dataType_; };

    /** @return the number of bytes required to represent this data type */
    std::size_t getRepresentationByteLength() const;

    /** @return true if the view is equivalent */
    bool operator==(const FileView& other) const;

private:
    /** Swap the contents of this and other */
    void swap(FileView& other);

    FSOffset displacement_;

    DataType* dataType_;
};

/** @return Output stream containg the file view */
std::ostream& operator<<(std::ostream& ost, const FileView& fv);

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
