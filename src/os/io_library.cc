/**
 * @file io_library.cc
 *
 * Note: These classes are adapted from the FSS simulator project and are
 * licensed only under the GPL.  The FSS project is available at:
 * http://www.omnetpp.org/filemgmt/singlefile.php?lid=104 and
 * http://www.omnetpp.org/doc/FSS-doc/neddoc/index.html
 */

#include <cassert>
#include <iostream>
#include "file_system.h"
#include "os_proto_m.h"
#include "io_library.h"
using namespace std;

void AbstractIOLibrary::initialize()
{
  fromInGateId = gate("in")->id();
}

void AbstractIOLibrary::handleMessage( cMessage *msg )
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

Define_Module_Like( PassThroughIOLibrary, AIOLibrary )


//-------------------------------------------------
Define_Module_Like(PFSIOLibrary, AIOLibrary);

void PFSIOLibrary::initialize()
{
    inGateId_ = gate("in")->id();
    requestGateId_ = gate("request")->id();
    outGateId_ = gate("out")->id();
    localFileSystem_ = dynamic_cast<NativeFileSystem*>(
        parentModule()->submodule("fileSystem"));
    assert(0 != localFileSystem_);
}

void PFSIOLibrary::handleMessage(cMessage* msg)
{
    if (msg->arrivalGateId() == inGateId_)
    {
        switch(msg->kind())
        {
            case SPFS_OS_FILE_READ_REQUEST:
            {
                spfsOSFileReadRequest* read =
                    dynamic_cast<spfsOSFileReadRequest*>(msg);
                
                FSHandle handle = read->getFileHandle();
                size_t offset = read->getOffset();
                size_t bytes = read->getExtent();
                vector<long> blocks =
                    localFileSystem_->getBlocks(handle, offset, bytes);
                vector<long>::iterator iter;
                for (iter = blocks.begin(); iter != blocks.end(); iter++)
                {
                    cMessage* fssReadReq = new cMessage();
                    fssReadReq->addPar("block").setLongValue(*iter);
                    fssReadReq->addPar("priority").setLongValue(-1);
                    fssReadReq->addPar("is_read").setBoolValue(true);
                    fssReadReq->addPar("jobId").setLongValue(read->getJobId());
                    send(fssReadReq, requestGateId_);
                }
                break;
            }
            case SPFS_OS_FILE_WRITE_REQUEST:
            {
                spfsOSFileWriteRequest* write =
                    dynamic_cast<spfsOSFileWriteRequest*>(msg);
                
                FSHandle handle = write->getFileHandle();
                size_t offset = write->getOffset();
                size_t bytes = write->getExtent();
                vector<long> blocks =
                    localFileSystem_->getBlocks(handle, offset, bytes);
                vector<long>::iterator iter;
                for (iter = blocks.begin(); iter != blocks.end(); iter++)
                {
                    cMessage* fssWriteReq = new cMessage();
                    fssWriteReq->addPar("block").setLongValue(*iter);
                    fssWriteReq->addPar("priority").setLongValue(-1);
                    fssWriteReq->addPar("is_read").setBoolValue(false);
                    fssWriteReq->addPar("jobId").setLongValue(
                        write->getJobId());
                    send(fssWriteReq, requestGateId_);
                }
                break;
            }
            default:
                cerr << "Unrecognized message kind on in gate." << endl;
        }
    }
    else
    {
        // Translate outgoing message into an OS message
        bool isRead = msg->par("is_read").boolValue();
        if (isRead)
        {
            spfsOSFileReadResponse* readResp =
                new spfsOSFileReadResponse(0, SPFS_OS_FILE_READ_RESPONSE);
            readResp->setFileHandle(0);
            send(readResp, outGateId_);
        }
        else
        {
            spfsOSFileWriteResponse* writeResp =
                new spfsOSFileWriteResponse(0, SPFS_OS_FILE_WRITE_RESPONSE);
            writeResp->setFileHandle(0);
            send(writeResp, outGateId_);
        }
    }

    // Cleanup
    delete msg;
}

/*
 * Local variables:
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 *
 * vim: ts=8 sts=4 sw=4 expandtab
 */
