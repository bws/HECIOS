#include <iostream>
#include "IPvXAddress.h"
#include "TCPSocket.h"
#include "TCPSocketMap.h"
#include "mpiio_proto_m.h"
#include "pvfs_proto_m.h"
#include "umd_io_trace.h"
#include <omnetpp.h>
using namespace std;

/**
 * Model of a file system client library.
 */
class BMITcpClient : public cSimpleModule, public TCPSocket::CallbackInterface
{
public:
    /** Constructor */
    BMITcpClient() : cSimpleModule() {};
    
protected:
    /** Implementation of initialize */
    virtual void initialize();

    /** Implementation of finish */
    virtual void finish() {};

    /** Implementation of handleMessage */
    virtual void handleMessage(cMessage* msg);

    /** */
    void socketDataArrived(int, void *, cMessage *msg, bool);
    
private:

    /** @return a socket with an open connection to the server */
    TCPSocket* getConnectedSocket(int handle);
    
    int connectPort_;
    TCPSocketMap socketConnectionMap_;
};

// OMNet Registriation Method
Define_Module(BMITcpClient);

/**
 *
 */
void BMITcpClient::initialize()
{
    connectPort_ = par("connectPort").longValue();
}

/**
 * Handle MPI-IO Response messages
 */
void BMITcpClient::handleMessage(cMessage* msg)
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
    
    TCPSocket* socket = socketConnectionMap_.findSocketFor(msg);
    if (0 != socket)
    {
        socket->processMessage(msg); // dispatch to socketXXXX() methods
    }
    else
    {
        switch(msg->kind())
        {
            case FS_CREATE_REQUEST:
            cerr << "handleMessage not yet implemented for kind: "
                 << msg->kind() << endl;
            break;
        case FS_REMOVE_REQUEST:
            cerr << "handleMessage not yet implemented for kind: "
                 << msg->kind() << endl;
            break;
        case FS_READ_REQUEST:
            cerr << "handleMessage not yet implemented for kind: "
                 << msg->kind() << endl;
            break;
        case FS_WRITE_REQUEST:
             cerr << "handleMessage not yet implemented for kind: "
                 << msg->kind() << endl;
           break;
        case FS_GET_ATTR_REQUEST:
            cerr << "handleMessage not yet implemented for kind: "
                 << msg->kind() << endl;
            break;
        case FS_SET_ATTR_REQUEST:
            cerr << "handleMessage not yet implemented for kind: "
                 << msg->kind() << endl;
            break;
        case FS_LOOKUP_PATH_REQUEST:
            cerr << "handleMessage not yet implemented for kind: "
                 << msg->kind() << endl;
            break;
        case FS_CREATE_DIR_ENT_REQUEST:
            cerr << "handleMessage not yet implemented for kind: "
                 << msg->kind() << endl;
            break;
        case FS_REMOVE_DIR_ENT_REQUEST:
            cerr << "handleMessage not yet implemented for kind: "
                 << msg->kind() << endl;
            break;
        case FS_CHANGE_DIR_ENT_REQUEST:
            cerr << "handleMessage not yet implemented for kind: "
                 << msg->kind() << endl;
            break;
        case FS_TRUNCATE_REQUEST:
            cerr << "handleMessage not yet implemented for kind: "
                 << msg->kind() << endl;
            break;
        case FS_MAKE_DIR_REQUEST:
            cerr << "handleMessage not yet implemented for kind: "
                 << msg->kind() << endl;
            break;
        default:
            cerr << "handleMessage not yet implemented for kind: "
                 << msg->kind() << endl;
            break;
        }
    }
}

void BMITcpClient::socketDataArrived(int, void *, cMessage *msg, bool)
{
    ev << "Received TCP data, " << msg->byteLength() << " bytes\\n";
    delete msg;
}

TCPSocket* BMITcpClient::getConnectedSocket(int handle)
{
    TCPSocket* clientSocket = new TCPSocket();
    IPvXAddress serverAddress("192.168.1.1");

    clientSocket->connect(serverAddress, connectPort_);

    // Add open socket to bookkeeping maps
    socketConnectionMap_.addSocket(clientSocket);

    return clientSocket;
}

/*
 * Local variables:
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 *
 * vim: ts=8 sts=4 sw=4 expandtab
 */
