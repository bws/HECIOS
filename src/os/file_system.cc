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
#include <vector>
#include "os_proto_m.h"
using namespace std;

FileSystem::FileSystem()
    : cSimpleModule()
{
}

FileSystem::~FileSystem()
{
}

void FileSystem::initialize()
{
    inGateId_ = gate("in")->id();
    outGateId_ = gate("out")->id();
    requestGateId_ = gate("request")->id();
}

void FileSystem::finish()
{
}

void FileSystem::handleMessage(cMessage *msg)
{
    if (msg->arrivalGateId() == inGateId_)
    {
        // Convert the file system request into block requests
        vector<FSBlock> blocks = getRequestBlocks(msg);

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


// Register the NativeFileSystem type as a concrete FileSystem type
Define_Module_Like(NativeFileSystem, FileSystem);

vector<FSBlock> NativeFileSystem::getRequestBlocks(cMessage* msg) const
{
    std::vector<FSBlock> blocks;
    blocks.push_back(1);
    // for now assume all files start at block zero
    //long startBlock = offset % BLOCK_SIZE_BYTES;
    //long numBlocks = bytes % BLOCK_SIZE_BYTES;
    //for (int i = startBlock; i < numBlocks; i++)
    //{
    //    blocks.push_back(i);
    //}
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
