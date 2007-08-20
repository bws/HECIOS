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

#include "storage_layout_manager.h"
#include "filename.h"
using namespace std;

StorageLayoutManager* StorageLayoutManager::instance_ = 0;

StorageLayoutManager& StorageLayoutManager::instance()
{
    if (0 == instance_)
        instance_ = new StorageLayoutManager();
    return *instance_;
}

StorageLayoutManager::StorageLayoutManager()
{
}

// For now, simply assume OS blocks are 4KB and disk blocks are 512B
// Also simply assume the storage layout is contiguous
// Later, maybe add something sophisticated here
void StorageLayoutManager::addFile(std::size_t serverNumber,
                                   Filename& filename,
                                   FSSize size)
{
}

vector<FSBlock> StorageLayoutManager::getFileSystemBlocks(
    std::size_t serverNumber,
    FSOffset offset,
    FSSize extent)
{
    vector<FSBlock>blocks;
    blocks.push_back(1);
    return blocks;
}

/*
 * Local variables:
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 *
 * vim: ts=8 sts=4 sw=4 expandtab
 */
