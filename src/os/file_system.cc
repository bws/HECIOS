/**
 * @file file_system.cc
 *
 * Note: These classes are adapted from the FSS simulator project and are
 * licensed only under the GPL.  The FSS project is available at:
 * http://www.omnetpp.org/filemgmt/singlefile.php?lid=104 and
 * http://www.omnetpp.org/doc/FSS-doc/neddoc/index.html
 */

#include "file_system.h"

FileSystem::FileSystem()
    : cSimpleModule()
{
}

FileSystem::~FileSystem()
{
}

void FileSystem::initialize()
{
  fromInGateId = gate("in")->id();
}

void FileSystem::finish()
{
}

void FileSystem::handleMessage(cMessage *msg)
{
    if ( msg->arrivalGateId()==fromInGateId )
    {
        ev << className() << ": Sending " << msg->name() << " to request"
           << endl;
        send( msg, "request" );
    }
    else
    {
        ev << className() << ": Completed service of " << msg->name() << endl;
        send( msg, "out" );
    }
}


Define_Module_Like(NativeFileSystem, FileSystem);

std::vector<long> NativeFileSystem::getBlocks(FSHandle handle,
                                              size_t offset,
                                              size_t bytes)
{
    std::vector<long> blocks;

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
