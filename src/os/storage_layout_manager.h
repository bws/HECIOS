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

/**
 * Access manager for server physical data layout information
 *
 * This is a crappy piece of code that is virtual simply so I can test the
 * file builder and mainly exists to hunt down the file system for each
 * server.  It would probably be better to move some of this code into the
 * fs configurator at some level!!!
 *
 * Still unanswered is the question of whether a File is fragmented or a
 * file system is fragmented.  There is probably a clean way to resolve that
 * idea, but I don't need it yet, so for now I'm doing this!
 */
class StorageLayoutManagerIFace
{
public:
    /** Destructor */
    virtual ~StorageLayoutManagerIFace() {};
    
    /** Add a directory to a server's storage system */
    void addDirectory(std::size_t serverNumber,
                      const Filename& dirName);

    /** Add a file to a server's storage system */
    void addFile(std::size_t serverNumber,
                 const Filename& filename,
                 FSSize size);

protected:
    /** Add a directory to a server's native file system */
    virtual void addDirectoryToFS(std::size_t serverNumber,
                                  const Filename& filename) = 0;

    /** Add a file to a server's native file system */
    virtual void addFileToFS(std::size_t serverNumber,
                             const Filename& filename,
                             FSSize size) = 0;
};

class StorageLayoutManager : public StorageLayoutManagerIFace
{
protected:
    /** Add a directory to a server's native file system */
    virtual void addDirectoryToFS(std::size_t serverNumber,
                                  const Filename& filename);

    /** Add a file to a server's native file system */
    virtual void addFileToFS(std::size_t serverNumber,
                             const Filename& filename,
                             FSSize size);
    
private:
    /** @return the local file system for server number */
    FileSystem* getLocalFileSystem(std::size_t serverNumber) const;    
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
