#ifndef STORAGE_LAYOUT_MANAGER_H
#define STORAGE_LAYOUT_MANAGER_H
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

#include <cstddef>
#include <vector>
#include "basic_types.h"
class Filename;

/** */
class StorageLayoutManager
{
public:

    /** Singleton accessor */
    StorageLayoutManager& instance();

    /** Add a file to a server's native file system */
    void addFile(std::size_t serverNumber, Filename& filename, FSSize size);

    /** @return the file system blocks for a */
    std::vector<FSBlock> getFileSystemBlocks(std::size_t serverNumber,
                                             FSOffset offset,
                                             FSSize extent);

private:

    /** Constructor */
    StorageLayoutManager();

    /** Hidden copy constructor */
    StorageLayoutManager(const StorageLayoutManager& other);

    /** Hidden assignment operator */
    StorageLayoutManager& operator=(const StorageLayoutManager& other);

    static StorageLayoutManager* instance_;
};

#endif

/*
 * Local variables:
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 *
 * vim: ts=8 sts=4 sw=4 expandtab
 */