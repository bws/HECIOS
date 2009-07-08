#ifndef STORAGE_LAYOUT_H
#define STORAGE_LAYOUT_H
//
// This file is part of Hecios
//
// Copyright (C) 2007,2008,2009 Brad Settlemyer
//
// This file is distributed WITHOUT ANY WARRANTY. See the file 'License.txt'
// for details on this and other legal matters.
//
#include <map>
#include <vector>
#include "basic_types.h"
#include "filename.h"

/** An abstract storage layout interface */
class StorageLayout
{
public:

    /** Constructor */
    StorageLayout();

    /** Destructor */
    virtual ~StorageLayout() = 0;

    /** Add layout information for a directory */
    void addDirectory(const Filename& filename);

    /** Add layout information for a file */
    void addFile(const Filename& filename, FSSize size);

    /** @return vector of data blocks for a file's offset and extent */
    std::vector<FSBlock> getFileDataBlocks(const Filename& file,
                                           FSOffset offset,
                                           FSSize extent) const;

    /** @return vector of data blocks for a file and vector of file regions */
    std::vector<FSBlock> getFileDataBlocks(
        const Filename& file, std::vector<FileRegion> regions) const;

    /** @return the metadata blocks associated with a file */
    std::vector<FSBlock> getFileMetaDataBlocks(
        const Filename& file) const;

protected:
    /** Add layout information for a directory */
    virtual void addDirectoryToLayout(const Filename& filename) = 0;

    /** Add layout information for a file */
    virtual void addFileToLayout(const Filename& filename, FSSize size) = 0;

    /** @return vector of data blocks for a file and vector of file regions */
    virtual std::vector<FSBlock> getLayoutFileDataBlocks(
        const Filename& file, std::vector<FileRegion> regions) const = 0;

    /** @return the metadata blocks associated with a file */
    virtual std::vector<FSBlock> getLayoutFileMetaDataBlocks(
        const Filename& file) const = 0;

private:
    /** Copy constructor */
    StorageLayout( StorageLayout& other );

    /** Assignment */
    StorageLayout& operator=(StorageLayout& other );
};

#endif

/*
 * Local variables:
 *  indent-tabs-mode: nil
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 *
 * vim: ts=8 sts=4 sw=4 expandtab
 */
