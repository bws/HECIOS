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
#include "filename.h"
#include "simple_stripe_distribution.h"
#include "storage_layout_manager.h"
using namespace std;

FileBuilder* FileBuilder::instance_ = 0;

FileBuilder& FileBuilder::instance()
{
    if (0 == instance_)
        instance_ = new FileBuilder();
    return *instance_;
}

void FileBuilder::clearState()
{
    delete instance_;
    instance_ = 0;
}

FileBuilder::FileBuilder()
    : nextServerNumber_(0)
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

FSDescriptor* FileBuilder::getDescriptor(const Filename& fileName) const
{
    FSDescriptor* descriptor = 0;
    if (fileExists(fileName))
    {
        descriptor = new FSDescriptor();
        descriptor->metaData = getMetaData(fileName);
        descriptor->path = fileName.str();
        descriptor->filePtr = 0;
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
        // Create implied parent directories recursively
        size_t numSegs = dirName.getNumPathSegments();
        if (1 < numSegs)
        {
            createDirectory(dirName.getSegment(numSegs - 2),
                            metaServer,
                            layoutManager);
        }
        // Create the MetaData for the directory
        FSMetaData* meta = new FSMetaData();
        meta->mode = 777;
        meta->owner = 0;
        meta->group = 0;
        meta->nlinks = 0;
        meta->size = 0;
        meta->handle = getNextHandle(metaServer);
        meta->dist = 0;

        // Construct the storage layout for the directory metadata
        Filename storageName(meta->handle);
        layoutManager.addFile((size_t)metaServer,
                              storageName,
                              DEFAULT_METADATA_SIZE);

        // Record bookkeeping information
        nameToHandleMap_[dirName.str()] = meta->handle;
        handleToMetaMap_[meta->handle] = meta;
    }
}

void FileBuilder::createFile(const Filename& fileName,
                             int metaServer,
                             int numServers,
                             StorageLayoutManagerIFace& layoutManager)
{
    if (!fileExists(fileName))
    {
        // Create implied parent directories recursively
        size_t numSegs = fileName.getNumPathSegments();
        if (1 < numSegs)
        {
            createDirectory(fileName.getSegment(numSegs - 2),
                            metaServer,
                            layoutManager);
        }
        
        // Create the MetaData for the file
        FSMetaData* meta = new FSMetaData();
        meta->mode = 777;
        meta->owner = 0;
        meta->group = 0;
        meta->nlinks = 0;
        meta->size = 0;
        meta->handle = getNextHandle(metaServer);
        meta->dist = new SimpleStripeDistribution(0, numServers);

        // Construct the storage layout for the file metadata
        Filename storageMeta(meta->handle);
        layoutManager.addFile((size_t)metaServer, storageMeta,
                              DEFAULT_METADATA_SIZE);
        
        // Construct the data handles
        cerr << "Creating file on " << numServers << endl;
        int firstServer = rand() % nextServerNumber_;
        vector<FSHandle> dataHandles;
        for (size_t i = 0; i < nextServerNumber_; i++)
        {
            int serverNum = (firstServer + i) % nextServerNumber_;
            dataHandles.push_back(getNextHandle(serverNum));

            // Construct the storage layout for the PFS file
            Filename storageName(dataHandles[i]);
            FSSize localFileSize = (FSSize)pow(2.0, 20.0);
            layoutManager.addFile((size_t)serverNum, storageName, localFileSize);
        }
        meta->dataHandles = dataHandles;

        // Record bookeeping information
        nameToHandleMap_[fileName.str()] = meta->handle;
        handleToMetaMap_[meta->handle] = meta;
    }
    else
    {
        cerr << "Cannot create " << fileName << ": file exists" << endl;
    }
}

/*
 * Local variables:
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 *
 * vim: ts=8 sts=4 sw=4 expandtab
 */
