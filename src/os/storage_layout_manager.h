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
#include <map>
#include <vector>
#include "basic_types.h"
class Filename;
class FileSystem;
class StorageLayout;

/** Manager for server physical data layout information */
class StorageLayoutManager
{
public:
    /** Convenience typedef */
    typedef std::map<std::size_t, StorageLayout*> ServerToStorageMap;
    
    /** Singleton accessor */
    static StorageLayoutManager& instance();

    /** Add a directory to a server's native file system */
    void addDirectory(std::size_t serverNumber,
                      const Filename& filename);

    /** Add a file to a server's native file system */
    void addFile(std::size_t serverNumber,
                 const Filename& filename,
                 FSSize size);

private:

    /** Constructor */
    StorageLayoutManager();

    /** Hidden copy constructor */
    StorageLayoutManager(const StorageLayoutManager& other);

    /** Hidden assignment operator */
    StorageLayoutManager& operator=(const StorageLayoutManager& other);

    FileSystem* getLocalFileSystem(std::size_t serverNumber) const;
    
    /** Singleton instance */
    static StorageLayoutManager* instance_;

    /** Map to the storage layout for a server */
    ServerToStorageMap storageLayoutByServer_;
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
