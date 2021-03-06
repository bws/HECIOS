//
// This file is part of Hecios
//
// Copyright (C) 2007,2008,2009 Brad Settlemyer
//
// This file is distributed WITHOUT ANY WARRANTY. See the file 'License.txt'
// for details on this and other legal matters.
//
#include "fixed_inode_storage_layout.h"
#include <algorithm>
#include <cassert>
using namespace std;

FixedINodeStorageLayout::FixedINodeStorageLayout(size_t blockSize)
    : StorageLayout(),
      fsBlockSize_(blockSize)
{
    assert(0 < fsBlockSize_);

    // Assume the IONodes are in blocks 0 - 999
    nextMetaDataBlock_ = 0;
    nextDataBlock_ = 1000;
}

void FixedINodeStorageLayout::addDirectoryToLayout(const Filename& dirName)
{
    // Assign a single block for storing directory metadata
    metaDataBlocks_[dirName] = nextMetaDataBlock_++;

    // Assign a fixed number of blocks for storing directory entries
    dataBlocks_[dirName] = nextDataBlock_;
    nextDataBlock_ += NUM_DIRECTORY_DATA_BLOCKS;
}

void FixedINodeStorageLayout::addFileToLayout(const Filename& filename,
                                              FSSize fileSize)
{
    // Assign a single block for storing the inodes
    metaDataBlocks_[filename] = nextMetaDataBlock_++;

    // Assign contiguous data blocks to the file
    int64_t numBlocks = fileSize / fsBlockSize_;
    dataBlocks_[filename] = nextDataBlock_;
    nextDataBlock_ += numBlocks;

}

vector<FSBlock> FixedINodeStorageLayout::getLayoutFileMetaDataBlocks(
    const Filename& filename) const
{
    // Locate the file's inode block
    std::map<Filename, FSBlock>::const_iterator iter =
        metaDataBlocks_.find(filename);
    assert(metaDataBlocks_.end() != iter);

    // Return the inode block
    vector<FSBlock> blocks(1);
    blocks[0] = iter->second;
    return blocks;
}

vector<FSBlock> FixedINodeStorageLayout::getLayoutFileDataBlocks(
    const Filename& filename, vector<FileRegion> regions) const
{
    vector<FSBlock> blocks;

    // Locate the file's first block
    std::map<Filename, FSBlock>::const_iterator iter =
        dataBlocks_.find(filename);
    assert(dataBlocks_.end() != iter);
    FSBlock firstFileBlock = iter->second;

    // Process each file region
    for (size_t i = 0; i < regions.size(); i++)
    {
        FSOffset offset = regions[i].offset;
        FSSize extent = regions[i].extent;

        // Find the offset block
        int64_t blocksOffset = offset / fsBlockSize_;

        // Calculate the number of full blocks to access
        uint64_t blocksToAccess = (extent/fsBlockSize_);

        // Add a block for any partial block access at beginning of request
        FSSize partialBlock =
            (blocksOffset * fsBlockSize_ - offset) % fsBlockSize_;
        FSSize firstBlockPortion = min(extent, partialBlock);
        if (0 != firstBlockPortion)
        {
            blocksToAccess += 1;
        }

        // Add a block for any partial block access at end of request
        int64_t lastBlockPortion = (extent - firstBlockPortion) % fsBlockSize_;
        if (0 != lastBlockPortion)
        {
            blocksToAccess += 1;
        }

        // Add region's blocks the list of blocks to access
        FSBlock firstBlock = firstFileBlock + blocksOffset;
        for (uint64_t i = 0; i < blocksToAccess; i++)
        {
            blocks.push_back(firstBlock + i);
        }
    }
    return blocks;
}

/*
 * Local variables:
 *  indent-tabs-mode: nil
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 *
 * vim: ts=8 sts=4 sw=4 expandtab
 */

