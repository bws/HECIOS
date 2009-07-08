//
// This file is part of Hecios
//
// Copyright (C) 2007,2008,2009 Brad Settlemyer
//
// This file is distributed WITHOUT ANY WARRANTY. See the file 'License.txt'
// for details on this and other legal matters.
//
#include "storage_layout.h"
#include <cassert>
using namespace std;

StorageLayout::StorageLayout()
{
}

StorageLayout::~StorageLayout()
{
}

void StorageLayout::addDirectory(const Filename& dirName)
{
    addDirectoryToLayout(dirName);
}

void StorageLayout::addFile(const Filename& filename, FSSize fileSize)
{
    addFileToLayout(filename, fileSize);
}

vector<FSBlock> StorageLayout::getFileMetaDataBlocks(
    const Filename& filename) const
{
    return getLayoutFileMetaDataBlocks(filename);
}

vector<FSBlock> StorageLayout::getFileDataBlocks(const Filename& filename,
                                                 FSOffset offset,
                                                 FSSize extent) const
{
    // Create a file region
    FileRegion fr = {offset, extent};
    vector<FileRegion> regions(1);
    regions.push_back(fr);

    return getFileDataBlocks(filename, regions);
}

vector<FSBlock> StorageLayout::getFileDataBlocks(
    const Filename& filename, vector<FileRegion> regions) const
{
    return getLayoutFileDataBlocks(filename, regions);
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

