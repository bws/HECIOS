//
// This file is part of Hecios
//
// Copyright (C) 2007,2008,2009 Brad Settlemyer
//
// This file is distributed WITHOUT ANY WARRANTY. See the file 'License.txt'
// for details on this and other legal matters.
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
            cModule* osMod = ionMod->submodule("os");
            assert(0 != osMod);

            cModule* fileSystemMod = osMod->submodule("fileSystem");
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
