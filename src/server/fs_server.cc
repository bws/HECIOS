#include <iostream>
#include "IPAddressResolver.h"
#include "IPvXAddress.h"
#include "mpiio_proto_m.h"
#include "pvfs_proto_m.h"
#include "pfs_types.h"
#include "pfs_utils.h"
#include <omnetpp.h>
using namespace std;

/**
 * Model of a file system client library.
 */
class FSServer : public cSimpleModule
{
public:
    /** Constructor */
    FSServer() : cSimpleModule() {};
    
protected:
    /** Overload the number of init stages */
    int numInitStages() const { return 2; };
    
    /** Implementation of initialize */
    virtual void initialize(int stage);

    /** Implementation of finish */
    virtual void finish() {};

    /** Implementation of handleMessage */
    virtual void handleMessage(cMessage* msg);

private:

    /** */
    static int serverNumber_;

    /** */
    string serverName_;

    /** */
    HandleRange range_;
};

// OMNet Registriation Method
Define_Module(FSServer);

int FSServer::serverNumber_ = 0;

/**
 * Multi-stage initialization
 *
 * Stage 1 - Set the name and handle range
 * Stage 2 - register the assigned IP address
 */
void FSServer::initialize(int initStage)
{
    if (0 == initStage)
    {
        // Set the server's name
        stringstream s;
        s << serverNumber_;
        serverName_ = "server" + s.str();
    
        // Determine this server's handle range
        range_.first = serverNumber_ * 1000;
        range_.last = range_.first + 999;

        // Increment the server number counter
        serverNumber_++;
    }
    else if (1 == initStage)
    {
        // Register the IP for this server's handles
        //cModule* hcaModule = parentModule()->parentModule()->submodule("hca");
        //cerr << "Found Module: " << hcaModule->fullName() <<endl;
        //IPvXAddress addr = IPAddressResolver().addressOf(
        //    hcaModule, IPAddressResolver::ADDR_PREFER_IPv4);
        //cerr << serverName_ << " IP: " << addr << endl;
        //PFSUtils& utils = PFSUtils::instance();
        //utils.registerServerIP(0, range_);
    }
}

/**
 * Handle MPI-IO Response messages
 */
void FSServer::handleMessage(cMessage* msg)
{
    // For now, construct the appropriate response and simply send it back
    cMessage* response;
    switch(msg->kind())
    {
        case MPI_FILE_OPEN_REQUEST:
        {
            response = new mpiFileOpenResponse(0, MPI_FILE_OPEN_RESPONSE);
            break;
        }
        case MPI_FILE_CLOSE_REQUEST:
        {
            response = new mpiFileCloseResponse(0, MPI_FILE_OPEN_RESPONSE);
            break;
        }
        case MPI_FILE_DELETE_REQUEST:
        {
            response = new mpiFileDeleteResponse(0, MPI_FILE_DELETE_RESPONSE);
            break;
        }
        case MPI_FILE_SET_SIZE_REQUEST:
        {
            response = new mpiFileSetSizeResponse(0,
                                                  MPI_FILE_SET_SIZE_RESPONSE);
            break;
        }
        case MPI_FILE_PREALLOCATE_REQUEST:
        {
            response = new mpiFilePreallocateResponse(
                0, MPI_FILE_PREALLOCATE_RESPONSE);
            break;
        }
        case MPI_FILE_GET_SIZE_REQUEST:
        {
            response = new mpiFileGetSizeResponse(0,
                                                  MPI_FILE_GET_SIZE_RESPONSE);
            break;
        }
        case MPI_FILE_GET_INFO_REQUEST:
        {
            response = new mpiFileGetInfoResponse(0,
                                                  MPI_FILE_GET_INFO_RESPONSE);
            break;
        }
        case MPI_FILE_SET_INFO_REQUEST:
        {
            response = new mpiFileSetInfoResponse(0, MPI_FILE_OPEN_RESPONSE);
            break;
        }
        case MPI_FILE_READ_AT_REQUEST:
        {
            response = new mpiFileReadAtResponse(0, MPI_FILE_READ_AT_RESPONSE);
            break;
        }
        case MPI_FILE_READ_REQUEST:
        {
            response = new mpiFileReadResponse(0, MPI_FILE_READ_RESPONSE);
            break;
        }
        case MPI_FILE_WRITE_AT_REQUEST:
        {
            response = new mpiFileWriteAtResponse(0,
                                                  MPI_FILE_WRITE_AT_RESPONSE);
            break;
        }
        case MPI_FILE_WRITE_REQUEST:
        {
            response = new mpiFileWriteResponse(0, MPI_FILE_WRITE_RESPONSE);
            break;
        }
    }

    delete msg;
    send(response, "bmiOut");
    return;
}

/*
 * Local variables:
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 *
 * vim: ts=8 sts=4 sw=4 expandtab
 */
