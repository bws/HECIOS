/**
 * @file file_system.cc
 *
 * Note: These classes are adapted from the FSS simulator project and are
 * licensed only under the GPL.  The FSS project is available at:
 * http://www.omnetpp.org/filemgmt/singlefile.php?lid=104 and
 * http://www.omnetpp.org/doc/FSS-doc/neddoc/index.html
 */

#include "file_system.h"

AbstractFileSystem::AbstractFileSystem( const char *namestr, cModule *parent, size_t stack):
cSimpleModule( namestr, parent, stack),
queue("queue")
{
}

void AbstractFileSystem::initialize()
{
  fromInGateId = gate("in")->id();
}

void AbstractFileSystem::finish()
{
}

void AbstractFileSystem::handleMessage(cMessage *msg)
{
  if ( msg->arrivalGateId()==fromInGateId ) {
    ev << className() << ": Sending " << msg->name() << " to request" << endl;
    send( msg, "request" );
  } else {
    ev << className() << ": Completed service of " << msg->name() << endl;
    send( msg, "out" );
  }
}

//------------------------------------------------

Define_Module_Like( PassThroughFileSystem, AFileSystem )

PassThroughFileSystem::
    PassThroughFileSystem(const char *namestr, cModule *parent, size_t stack):
AbstractFileSystem(namestr, parent, stack)
{
}


Define_Module_Like(NativeFileSystem, AFileSystem);

std::vector<long> NativeFileSystem::getBlocks(FSHandle handle,
                                              size_t offset,
                                              size_t bytes)
{
    std::vector<long> blocks;

    // for now assume all files start at block zero
    long startBlock = offset % BLOCK_SIZE_BYTES;
    long numBlocks = bytes % BLOCK_SIZE_BYTES;
    for (int i = startBlock; i < numBlocks; i++)
    {
        blocks.push_back(i);
    }
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
