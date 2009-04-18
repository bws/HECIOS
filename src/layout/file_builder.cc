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

#include "file_builder.h"
#include <cassert>
#include <cmath>
#include <cstdlib>
#include "IPvXAddress.h"
#include "file_descriptor.h"
#include "filename.h"
#include "simple_stripe_distribution.h"
#include "storage_layout_manager.h"
using namespace std;

FileBuilder::FileBuilder()
    : defaultMetaDataSize_(0),
      nextServerNumber_(0)
{
}

FileBuilder::~FileBuilder()
{
    map<FSHandle, FSMetaData*>::const_iterator iter = handleToMetaMap_.begin();
    while (handleToMetaMap_.end() != iter)
    {
        FSMetaData* meta = iter->second;
        delete meta->dist;
        delete meta;
        ++iter;
    }
}

void FileBuilder::setDefaultMetaDataSize(size_t metaDataSize)
{
    defaultMetaDataSize_ = metaDataSize;
}

int FileBuilder::registerFSServer(const HandleRange& range, bool isMetaServer)
{
    assert(nextServerNumber_ == handlesByServer_.size());
    assert(nextServerNumber_ == nextHandleByServer_.size());
    handlesByServer_.push_back(range);
    nextHandleByServer_.push_back(range.first);

    if (isMetaServer)
        metaServers_.push_back(nextServerNumber_);

    return nextServerNumber_++;
}

bool FileBuilder::fileExists(const Filename& fileName) const
{
    std::map<std::string, FSHandle>::const_iterator pos;
    pos = nameToHandleMap_.find(fileName.str());
    return (nameToHandleMap_.end() != pos);
}

vector<int> FileBuilder::getMetaServers() const
{
    return metaServers_;
}

FSMetaData* FileBuilder::getMetaData(const Filename& fileName) const
{
    FSMetaData* md = 0;
    std::map<std::string, FSHandle>::const_iterator p1;
    p1 = nameToHandleMap_.find(fileName.str());
    if (nameToHandleMap_.end() != p1)
    {
        std::map<FSHandle, FSMetaData*>::const_iterator p2;
        p2 = handleToMetaMap_.find(p1->second);
        md = p2->second;
    }

    return md;
}

FSMetaData* FileBuilder::getMetaData(const FSHandle& handle) const
{
    FSMetaData* md = 0;

    std::map<FSHandle, FSMetaData*>::const_iterator iter;
    iter = handleToMetaMap_.find(handle);
    if (handleToMetaMap_.end() != iter)
    {
        md = iter->second;
    }
    return md;
}

FileDescriptor* FileBuilder::getDescriptor(const Filename& fileName) const
{
    FileDescriptor* descriptor = 0;
    if (fileExists(fileName))
    {
        FSMetaData* meta = getMetaData(fileName);
        descriptor = new FileDescriptor(fileName, *meta);
    }
    return descriptor;
}

FSHandle FileBuilder::getFirstHandle(size_t serverNumber)
{
    assert(serverNumber < nextServerNumber_);
    return handlesByServer_[serverNumber].first;
}

FSHandle FileBuilder::getNextHandle(size_t serverNumber)
{
    assert(serverNumber < nextServerNumber_);
    return nextHandleByServer_[serverNumber]++;
}

void FileBuilder::createDirectory(const Filename& dirName,
                                  int metaServer,
                                  StorageLayoutManagerIFace& layoutManager)
{
    if (!fileExists(dirName))
    {
        //cerr << "Creating Dir: " << dirName << " Meta:" << metaServer <<endl;
        // Create implied parent directories recursively
        size_t numSegs = dirName.getNumPathSegments();
        if (1 < numSegs)
        {
            // Place the nested directory on the next meta Server
            int nextMeta = (metaServer + 1) % metaServers_.size();
            createDirectory(dirName.getSegment(numSegs - 2),
                            nextMeta,
                            layoutManager);
        }
        // Create the MetaData for the directory
        FSMetaData* meta = new FSMetaData();
        meta->mode = 777;
        meta->owner = 0;
        meta->group = 0;
        meta->nlinks = 0;
        meta->size = DEFAULT_DIRECTORY_SIZE;
        meta->handle = getNextHandle(metaServer);
        meta->dist = 0;

        // Construct the storage layout for the directory metadata
        Filename storageName(meta->handle);
        layoutManager.addFile((size_t)metaServer,
                              storageName,
                              defaultMetaDataSize_);

        // Construct the data handle for storing directory entries
        int directoryDataHandle = getNextHandle(metaServer);
        vector<FSHandle> dataHandles;
        dataHandles.push_back(directoryDataHandle);
        meta->dataHandles = dataHandles;

        // Construct the storage layout for the PFS file
        Filename direntName(directoryDataHandle);
        layoutManager.addFile(metaServer, direntName, meta->size);

        // Record bookkeeping information
        nameToHandleMap_[dirName.str()] = meta->handle;
        handleToMetaMap_[meta->handle] = meta;
    }
}

void FileBuilder::createFile(const Filename& fileName,
                             FSSize fileSize,
                             int metaServer,
                             int numServers,
                             StorageLayoutManagerIFace& layoutManager)
{
    if (!fileExists(fileName))
    {
        //cerr << "Creating file: " << fileName
        //     << " NumObjs:" << numServers <<endl;
        // Create implied parent directories recursively
        size_t numSegs = fileName.getNumPathSegments();
        if (1 < numSegs)
        {
            int nextMeta = (metaServer + 1) % metaServers_.size();
            createDirectory(fileName.getSegment(numSegs - 2),
                            nextMeta,
                            layoutManager);
        }

        // Create the MetaData for the file
        FSMetaData* meta = new FSMetaData();
        meta->mode = 777;
        meta->owner = 0;
        meta->group = 0;
        meta->nlinks = 0;
        meta->size = fileSize;
        meta->handle = getNextHandle(metaServer);
        meta->dist = new SimpleStripeDistribution(0, numServers);

        // Construct the storage layout for the file metadata
        Filename storageMeta(meta->handle);
        layoutManager.addFile((size_t)metaServer, storageMeta,
                              defaultMetaDataSize_);

        // Construct the data handles
        int firstServer = rand() % nextServerNumber_;
        vector<FSHandle> dataHandles;
        for (size_t i = 0; i < nextServerNumber_; i++)
        {
            int serverNum = (firstServer + i) % nextServerNumber_;
            FSHandle dataHandle = getNextHandle(serverNum);

            // Construct the storage layout for the PFS file
            Filename storageName(dataHandle);
            FSSize localFileSize = meta->size / numServers;
            if (0 != localFileSize)
            {
                layoutManager.addFile((size_t)serverNum, storageName, localFileSize);
            }
            else
            {
                layoutManager.addFile((size_t)serverNum, storageName, DEFAULT_BSTREAM_SIZE);
            }

            // Update the metadata
            meta->dataHandles.push_back(dataHandle);
            meta->bstreamSizes.push_back(localFileSize);
        }

        // Record bookeeping information
        nameToHandleMap_[fileName.str()] = meta->handle;
        handleToMetaMap_[meta->handle] = meta;
    }
    else
    {
        cerr << "ERROR: Cannot create " << fileName << ": file exists" << endl;
    }
}

size_t FileBuilder::getNumDataObjects(const FSHandle& metaHandle) const
{
    std::map<FSHandle, FSMetaData*>::const_iterator pos =
        handleToMetaMap_.find(metaHandle);
    assert(handleToMetaMap_.end() != pos);
    return pos->second->dataHandles.size();
}

void FileBuilder::populateFileSystem(const FileSystemMap& traceFS)
{
    StorageLayoutManager layoutManager;

    FileSystemMap::const_iterator iter = traceFS.begin();
    cerr << "DIAGNOSTIC: Trace file system contains: "
         << traceFS.size() << " directories and files\n";
    for (size_t i = 0; i < traceFS.size(); i++)
    {
        // Determine the meta server using a round robin scheme
        size_t metaIdx = i % metaServers_.size();

        Filename filename(iter->first);
        FSSize fileSize(iter->second);
        assert(0 <= fileSize);

        // Create the file using all of the data servers
        createFile(filename, fileSize,
                   metaServers_[metaIdx],
                   getNumDataServers(),
                   layoutManager);

        // Increment to next file
        ++iter;
    }
}

void FileBuilder::populateFileSystem(const FileSystemMap& traceDirs,
                                     const FileSystemMap& traceFiles)
{
    StorageLayoutManager layoutManager;
    size_t totalCount = 0;

    // First build the directories
    FileSystemMap::const_iterator dirIter = traceDirs.begin();
    cerr << "DIAGNOSTIC: Trace file system contains: "
         << traceDirs.size() << " directories\n";
    for (size_t i = 0; i < traceDirs.size(); i++)
    {
        // Determine the meta server using a round robin scheme
        size_t metaIdx = totalCount++ % metaServers_.size();

        Filename dirName(dirIter->first);
        //size_t numEntries(dirIter->second);

        // Create the directory
        createDirectory(dirName, metaServers_[metaIdx], layoutManager);

        // Increment to next directory
        ++dirIter;
    }

    FileSystemMap::const_iterator fileIter = traceFiles.begin();
    cerr << "DIAGNOSTIC: Trace file system contains: "
         << traceFiles.size() << " files\n";
    for (size_t i = 0; i < traceFiles.size(); i++)
    {
        // Determine the meta server using a round robin scheme
        size_t metaIdx = totalCount++ % metaServers_.size();

        Filename filename(fileIter->first);
        FSSize fileSize(fileIter->second);

        // Create the file
        createFile(filename,
                   fileSize,
                   metaServers_[metaIdx],
                   getNumDataServers(),
                   layoutManager);

        // Increment to next file
        ++fileIter;
    }


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
