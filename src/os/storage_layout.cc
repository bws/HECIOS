//
// This file is part of Hecios
//
// Copyright (C) 2007 Brad Settlemyer
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
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

