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
    inGateId_ = findGate("in");
    outGateId_ = findGate("out");
    requestGateId_ = findGate("request");

    // Initialize concrete file system
    initializeFileSystem();
}

void FileSystem::finish()
{
    finishFileSystem();
}

//
// First retrieve the metadata to determine where the data blocks are located
// Then retrieve the actual data blocks
//
void FileSystem::handleMessage(cMessage *msg)
{
    // If the message is a new client request, process it directly
    // Otherwise its a response, extract the originating request
    // and then process the response
    if (msg->arrivalGateId() == inGateId_)
    {
        processMessage(dynamic_cast<spfsOSFileIORequest*>(msg), msg);
    }
    else
    {
        cMessage* parentReq = static_cast<cMessage*>(msg->contextPointer());
        spfsOSFileIORequest* origRequest =
            static_cast<spfsOSFileIORequest*>(parentReq->contextPointer());
        processMessage(origRequest, msg);
        delete parentReq;
        delete msg;
    }

}

void FileSystem::processMessage(spfsOSFileIORequest* request, cMessage* msg)
{
    // Restore the existing state for this request
    assert(0 != request);
    cFSM currentState = request->getState();

    // File System I/O states
    enum {
        INIT = 0,
        READ_META = FSM_Steady(1),
        SEND_IO_REQUEST = FSM_Transient(2),
        WRITE_META = FSM_Steady(3),
        IO_COMPLETE = FSM_Steady(4),
        META_COMPLETE = FSM_Steady(5),
        FINISH = FSM_Steady(7),
    };

    FSM_Switch(currentState)
    {
        case FSM_Exit(INIT):
        {
            assert(0 != dynamic_cast<spfsOSFileIORequest*>(msg));
            FSM_Goto(currentState, READ_META);
            break;
        }
        case FSM_Enter(READ_META):
        {
            assert(0 != dynamic_cast<spfsOSFileIORequest*>(msg));
            readMetaData(request);
            break;
        }
        case FSM_Exit(READ_META):
        {
            assert(0 != dynamic_cast<spfsOSReadBlocksResponse*>(msg));
            FSM_Goto(currentState, SEND_IO_REQUEST);
            break;
        }
        case FSM_Enter(SEND_IO_REQUEST):
        {
            assert(0 != dynamic_cast<spfsOSReadBlocksResponse*>(msg));
            performIO(request);
            break;
        }
        case FSM_Exit(SEND_IO_REQUEST):
        {
            assert(0 != dynamic_cast<spfsOSReadBlocksResponse*>(msg));
            FSM_Goto(currentState, WRITE_META);
            break;
        }
        case FSM_Enter(WRITE_META):
        {
            assert(0 != dynamic_cast<spfsOSReadBlocksResponse*>(msg));
            writeMetaData(request);
            break;
        }
        case FSM_Exit(WRITE_META):
        {
            // Note this is a little misleading, but it works okay
            // The point is we need both the io and meta to finish
            // before sending the final response
            if (0 == dynamic_cast<spfsOSReadBlocksResponse*>(msg))
            {
                FSM_Goto(currentState, IO_COMPLETE);
            }
            else
            {
                FSM_Goto(currentState, META_COMPLETE);
            }
            break;
        }
        case FSM_Exit(IO_COMPLETE):
        {
            FSM_Goto(currentState, FINISH);
            break;
        }
        case FSM_Exit(META_COMPLETE):
        {
            FSM_Goto(currentState, FINISH);
            break;
        }
        case FSM_Enter(FINISH):
        {
            sendFileIOResponse(request);
            break;
        }
    }

    // Store current state
    request->setState(currentState);
}

void FileSystem::readMetaData(spfsOSFileIORequest* ioRequest)
{
    // Lookup the metadata blocks
    Filename filename(ioRequest->getFilename());
    vector<FSBlock> blocks = getMetaDataBlocks(filename);
    assert(0 != blocks.size());

    // Construct the read message
    spfsOSReadBlocksRequest* readBlocks =
        new spfsOSReadBlocksRequest(0, SPFS_OS_READ_BLOCKS_REQUEST);
    readBlocks->setContextPointer(ioRequest);
    readBlocks->setBlocksArraySize(blocks.size());
    for (size_t i = 0; i < blocks.size(); i++)
        readBlocks->setBlocks(i, blocks[i]);
    send(readBlocks, requestGateId_);
}

void FileSystem::performIO(spfsOSFileIORequest* ioRequest)
{
    // Convert the file system request into block requests
    Filename filename(ioRequest->getFilename());
    FSOffset offset = ioRequest->getOffset();
    FSSize extent = ioRequest->getExtent();
    vector<FSBlock> blocks = getDataBlocks(filename, offset, extent);

    // Fill out the appropriate read or write message
    if (0 != dynamic_cast<spfsOSFileReadRequest*>(ioRequest))
    {
        spfsOSReadBlocksRequest* readBlocks =
            new spfsOSReadBlocksRequest(0, SPFS_OS_READ_BLOCKS_REQUEST);
        readBlocks->setContextPointer(ioRequest);
        readBlocks->setBlocksArraySize(blocks.size());
        for (size_t i = 0; i < blocks.size(); i++)
            readBlocks->setBlocks(i, blocks[i]);
        send(readBlocks, requestGateId_);
    }
    else
    {
        spfsOSWriteBlocksRequest* writeBlocks =
            new spfsOSWriteBlocksRequest(0, SPFS_OS_WRITE_BLOCKS_REQUEST);
        writeBlocks->setContextPointer(ioRequest);
        writeBlocks->setBlocksArraySize(blocks.size());
        for (size_t i = 0; i < blocks.size(); i++)
            writeBlocks->setBlocks(i, blocks[i]);
        send(writeBlocks, requestGateId_);
    }
}

void FileSystem::writeMetaData(spfsOSFileIORequest* ioRequest)
{
    // Lookup the metadata blocks
    Filename filename(ioRequest->getFilename());
    vector<FSBlock> blocks = getMetaDataBlocks(filename);
    assert(0 != blocks.size());
    
    // Write the first meta data block to simulate updating the atime
    spfsOSWriteBlocksRequest* writeBlock =
        new spfsOSWriteBlocksRequest(0, SPFS_OS_WRITE_BLOCKS_REQUEST);
    writeBlock->setContextPointer(ioRequest);
    writeBlock->setBlocksArraySize(1);
    writeBlock->setBlocks(0, blocks[0]);
    send(writeBlock, requestGateId_);
}

void FileSystem::sendFileIOResponse(spfsOSFileIORequest* ioRequest)
{
    // Respond to the read or write request
    if (0 != dynamic_cast<spfsOSFileReadRequest*>(ioRequest))
    {
        spfsOSFileReadResponse* resp =
            new spfsOSFileReadResponse(0, SPFS_OS_FILE_READ_RESPONSE);
        resp->setContextPointer(ioRequest);
        send(resp, outGateId_);
    }
    else
    {
        spfsOSFileWriteResponse* resp =
            new spfsOSFileWriteResponse(0, SPFS_OS_FILE_WRITE_RESPONSE);
        resp->setContextPointer(ioRequest);
        send(resp, outGateId_);
    }
}

// Register the NativeFileSystem type
Define_Module(NativeFileSystem);

void NativeFileSystem::initializeFileSystem()
{
    // Retrieve the block size
    blockSize_ = par("blockSizeBytes").longValue();
    
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

vector<FSBlock> NativeFileSystem::getMetaDataBlocks(
    const Filename& filename) const
{
    return storageLayout_->getFileMetaDataBlocks(filename);
}

vector<FSBlock> NativeFileSystem::getDataBlocks(
    const Filename& filename, FSOffset offset, FSSize extent) const
{
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
