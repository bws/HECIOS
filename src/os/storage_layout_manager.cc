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
#include <cassert>
#include <omnetpp.h>
#include "file_system.h"
#include "filename.h"
#include "fs_server.h"
#include "storage_layout.h"
using namespace std;

void StorageLayoutManagerIFace::addDirectory(size_t serverNumber,
                                        const Filename& dirName)
{
    addDirectoryToFS(serverNumber, dirName);
}

void StorageLayoutManagerIFace::addFile(size_t serverNumber,
                                   const Filename& filename,
                                   FSSize size)
{
    addFileToFS(serverNumber, filename, size);
}

void StorageLayoutManager::addDirectoryToFS(size_t serverNumber,
                                            const Filename& dirName)
{
    FileSystem* serverFS = getLocalFileSystem(serverNumber);
    assert(0 != serverFS);
    serverFS->createDirectory(dirName);
}

void StorageLayoutManager::addFileToFS(size_t serverNumber,
                                       const Filename& filename,
                                       FSSize size)
{
    FileSystem* serverFS = getLocalFileSystem(serverNumber);
    assert(0 != serverFS);
    serverFS->createFile(filename, size);
}

FileSystem* StorageLayoutManager::getLocalFileSystem(size_t serverNumber) const
{
    FileSystem* fs = 0;

    // Traverse the simulation module tree to find the server's file system
    cModule* clusterMod = simulation.systemModule();
    assert(0 != clusterMod);

    // Loop thru I/O Nodes
    int numIONodes = clusterMod->par("numIONodes");
    for (int i = 0; i < numIONodes; i++)
    {
        cModule* ionMod = clusterMod->submodule("ion", i);
        assert(0 != ionMod);

        cModule* daemonMod = ionMod->submodule("daemon");
        assert(0 != daemonMod);

        cModule* serverMod = daemonMod->submodule("pfsServer");
        assert(0 != serverMod);
        FSServer* fsServer = dynamic_cast<FSServer*>(serverMod);

        // If server numbers match, find the local file system
        if (serverNumber == fsServer->getNumber())
        {
            cModule* storageLayerMod = daemonMod->submodule("storage");
            assert(0 != storageLayerMod);

            cModule* fileSystemMod = storageLayerMod->submodule("fileSystem");
            fs = dynamic_cast<FileSystem*>(fileSystemMod);
            break;
        }
    }
    return fs;
}

/*
 * Local variables:
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 *
 * vim: ts=8 sts=4 sw=4 expandtab
 */
