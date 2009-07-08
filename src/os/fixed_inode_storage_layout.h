#ifndef FIXED_INODE_STORAGE_LAYOUT_H
#define FIXED_INODE_STORAGE_LAYOUT_H
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
#include "storage_layout.h"

/** Provides a fixed inode disk layout */
class FixedINodeStorageLayout : public StorageLayout
{
public:

    /** The number of data blocks to assign to a directory */
    static const std::size_t NUM_DIRECTORY_DATA_BLOCKS = 10;

    /** Constructor */
    FixedINodeStorageLayout(std::size_t blockSize);

protected:

    /** Add layout information for a directory */
    virtual void addDirectoryToLayout(const Filename& filename);

    /** Add layout information for a file */
    virtual void addFileToLayout(const Filename& filename, FSSize size);

    /** @return vector of data blocks for a file and vector of file regions */
    virtual std::vector<FSBlock> getLayoutFileDataBlocks(
        const Filename& file, std::vector<FileRegion> regions) const;

    /** @return the metadata blocks associated with a file */
    virtual std::vector<FSBlock> getLayoutFileMetaDataBlocks(
        const Filename& file) const;


private:

    /** Copy constructor hidden */
    FixedINodeStorageLayout( StorageLayout& other );

    /** Assignment operator hidden */
    FixedINodeStorageLayout operator=(StorageLayout& other );

    /** File system's block size */
    std::size_t fsBlockSize_;

    /** Next block to use for meta data */
    FSBlock nextMetaDataBlock_;

    /** Next block to use for file data */
    FSBlock nextDataBlock_;

    /** Map to the first inode block for a file */
    std::map<Filename, FSBlock> metaDataBlocks_;

    /** Map to the first data block for a file */
    std::map<Filename, FSBlock> dataBlocks_;
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
