//
// This file is part of Hecios
//
// Copyright (C) 2006 Joel Sherrill <joel@oarcorp.com>
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

#include "file_system.h"
#include <cassert>
#include <vector>
#include "filename.h"
#include "os_proto_m.h"
#include "storage_layout.h"
using namespace std;

FileSystem::FileSystem()
    : cSimpleModule()
{
}

FileSystem::~FileSystem()
{
}

void FileSystem::createDirectory(const Filename& dirName)
{
    // Assert name does not already exist
    allocateDirectoryStorage(dirName);
}

void FileSystem::createFile(const Filename& filename, FSSize size)
{
    // Assert name does not already exist
    allocateFileStorage(filename, size);
}

void FileSystem::initialize()
{
    inGateId_ = gate("in")->id();
    outGateId_ = gate("out")->id();
    requestGateId_ = gate("request")->id();

    // Initialize concrete file system
    initializeFileSystem();
}

void FileSystem::finish()
{
    finishFileSystem();
}

void FileSystem::handleMessage(cMessage *msg)
{
    if (msg->arrivalGateId() == inGateId_)
    {
        // Convert the file system request into block requests
        vector<FSBlock> blocks = getRequestBlocks(
            dynamic_cast<spfsOSFileIORequest*>(msg));

        // Fill out the appropriate read or write message
        if (0 != dynamic_cast<spfsOSFileReadRequest*>(msg))
        {
            spfsOSReadBlocksRequest* readBlocks =
                new spfsOSReadBlocksRequest(0, SPFS_OS_READ_BLOCKS_REQUEST);
            readBlocks->setContextPointer(msg);
            readBlocks->setBlocksArraySize(blocks.size());
            for (size_t i = 0; i < blocks.size(); i++)
                readBlocks->setBlocks(i, blocks[i]);
            send(readBlocks, requestGateId_);
        }
        else
        {
            spfsOSWriteBlocksRequest* writeBlocks =
                new spfsOSWriteBlocksRequest(0, SPFS_OS_WRITE_BLOCKS_REQUEST);
            writeBlocks->setContextPointer(msg);
            writeBlocks->setBlocksArraySize(blocks.size());
            for (size_t i = 0; i < blocks.size(); i++)
                writeBlocks->setBlocks(i, blocks[i]);
            send(writeBlocks, requestGateId_);
        }
    }
    else
    {
        cMessage* blockRequest = (cMessage*)msg->contextPointer();
        cMessage* parentRequest = (cMessage*)blockRequest->contextPointer();

        // Construct the correct response type
        spfsOSFileReadResponse* resp =
            new spfsOSFileReadResponse(0, SPFS_OS_FILE_READ_RESPONSE);
        resp->setContextPointer(parentRequest);
        send(resp, outGateId_);

        // Clean up the block request and response
        delete blockRequest;
        delete msg;
    }
}


// Register the NativeFileSystem type
Define_Module(NativeFileSystem);

void NativeFileSystem::initializeFileSystem()
{
    // Construct the storage layout for the file system
    storageLayout_ = new StorageLayout(getBlockSize());
}

void NativeFileSystem::finishFileSystem()
{
    // Free resources
    delete storageLayout_;
    storageLayout_ = 0;
}

void NativeFileSystem::allocateDirectoryStorage(const Filename& dirName)
{
    storageLayout_->addDirectory(dirName);
}

void NativeFileSystem::allocateFileStorage(const Filename& filename,
                                           FSSize size)
{
    storageLayout_->addFile(filename, size);
}

vector<FSBlock> NativeFileSystem::getRequestBlocks(
    spfsOSFileIORequest* ioRequest) const
{
    assert(0 != ioRequest);
    Filename filename("/home5/uysal/skirt.8p.matrix");
    FSOffset offset = ioRequest->getOffset();
    FSSize extent = ioRequest->getExtent();
    return storageLayout_->getFileDataBlocks(filename, offset, extent);
}

/*
 * Local variables:
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 *
 * vim: ts=8 sts=4 sw=4 expandtab
 */
