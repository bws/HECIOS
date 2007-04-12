#include <iostream>
#include "TCPSocket.h"
#include "TCPSocketMap.h"
#include "bmi_proto_m.h"
#include "mpiio_proto_m.h"
#include "pvfs_proto_m.h"
#include "umd_io_trace.h"
#include <omnetpp.h>
using namespace std;

/**
 * Model of a file system client library.
 */
class BMITcpServer : public cSimpleModule, public TCPSocket::CallbackInterface
{
public:
    /** Constructor */
    BMITcpServer() : cSimpleModule() {};
    
protected:
    /** Implementation of initialize */
    virtual void initialize();

    /** Implementation of finish */
    virtual void finish() {};

    /** Implementation of cSimpleModule::handleMessage */
    virtual void handleMessage(cMessage* msg);

    /** Implementation of TCPSocket::CallbackInterface::socketDataArrived */
    virtual void socketDataArrived(int connId, void* ptr, cMessage* msg,
                                   bool urgent);
    
private:

    /** The port to listen on */
    int listenPort_;

    /** Map for handling messages from open sockets */
    TCPSocketMap socketMap_;

    /** The server's listening socket */
    TCPSocket listenSocket_;

    /** Mapping from a messages uniqueId to the amount of data received */
    std::map<unsigned long,unsigned long> uidToAmountMap;

};

// OMNet Registriation Method
Define_Module(BMITcpServer);

void BMITcpServer::initialize()
{
    // Extract the port information
    listenPort_ = par("listenPort").longValue();

    // Setup the socket receive stuff
    listenSocket_.setOutputGate(gate("tcpOut"));
    listenSocket_.setCallbackObject(this, 0);
    
    // Open the server side socket
    listenSocket_.bind(listenPort_);
    listenSocket_.listen();
}

/**
 * Handle MPI-IO Response messages
 */
void BMITcpServer::handleMessage(cMessage* msg)
{
    // Attempt to treat the message as a TCP message
    // if that fails, then treat it as a regular message
    if (TCPSocket::belongsToAnyTCPSocket(msg))
    {
        TCPSocket* inSock = socketMap_.findSocketFor(msg);
        if (0 == inSock)
        {
            // Add the new incoming connection to the socket map
            inSock = new TCPSocket(msg);
            inSock->setOutputGate(gate("tcpOut"));
            inSock->setCallbackObject(this, NULL);
            socketMap_.addSocket(inSock);
        }
        inSock->processMessage(msg);
    }
    else
    {
        cMessage* response = 0;
        switch(msg->kind())
        {
            case SPFS_MPI_FILE_OPEN_REQUEST:
            {
                response = new spfsMPIFileOpenResponse(0, SPFS_MPI_FILE_OPEN_RESPONSE);
                break;
            }
            case SPFS_MPI_FILE_CLOSE_REQUEST:
            {
                response = new spfsMPIFileOpenResponse(0, SPFS_MPI_FILE_OPEN_RESPONSE);
                break;
            }
            case SPFS_MPI_FILE_DELETE_REQUEST:
            {
                response = new spfsMPIFileDeleteResponse(
                    0, SPFS_MPI_FILE_DELETE_RESPONSE);
                break;
            }
            case SPFS_MPI_FILE_SET_SIZE_REQUEST:
            {
                response = new spfsMPIFileSetSizeResponse(
                    0, SPFS_MPI_FILE_SET_SIZE_RESPONSE);
                break;
            }
            case SPFS_MPI_FILE_PREALLOCATE_REQUEST:
            {
                response = new spfsMPIFilePreallocateResponse(
                    0, SPFS_MPI_FILE_PREALLOCATE_RESPONSE);
                break;
            }
            case SPFS_MPI_FILE_GET_SIZE_REQUEST:
            {
                response =
                    new spfsMPIFileGetSizeResponse(0, SPFS_MPI_FILE_GET_SIZE_RESPONSE);
                break;
            }
            case SPFS_MPI_FILE_GET_INFO_REQUEST:
            {
                response =
                    new spfsMPIFileGetInfoResponse(0, SPFS_MPI_FILE_GET_INFO_RESPONSE);
                break;
            }
            case SPFS_MPI_FILE_SET_INFO_REQUEST:
            {
                 response =
                     new spfsMPIFileSetInfoResponse(0, SPFS_MPI_FILE_SET_INFO_RESPONSE);
               break;
            }
            case SPFS_MPI_FILE_READ_AT_REQUEST:
            {
                 response =
                     new spfsMPIFileReadAtResponse(0, SPFS_MPI_FILE_READ_AT_RESPONSE);
                break;
            }
            case SPFS_MPI_FILE_READ_REQUEST:
            {
                response =
                     new spfsMPIFileReadResponse(0, SPFS_MPI_FILE_READ_RESPONSE);
                break;
            }
            case SPFS_MPI_FILE_WRITE_AT_REQUEST:
            {
                response =
                     new spfsMPIFileWriteAtResponse(0, SPFS_MPI_FILE_WRITE_AT_RESPONSE);
                break;
            }
            case SPFS_MPI_FILE_WRITE_REQUEST:
            {
                response =
                     new spfsMPIFileWriteResponse(0, SPFS_MPI_FILE_WRITE_RESPONSE);
                break;
            }
            case SPFS_CREATE_REQUEST:
            {
                response = new spfsCreateResponse(0, SPFS_CREATE_RESPONSE);
                break;
            }
            case SPFS_REMOVE_REQUEST:
            case SPFS_READ_REQUEST:
            case SPFS_WRITE_REQUEST:
            case SPFS_GET_ATTR_REQUEST:
            case SPFS_SET_ATTR_REQUEST:
            case SPFS_LOOKUP_PATH_REQUEST:
            case SPFS_CREATE_DIR_ENT_REQUEST:
            case SPFS_REMOVE_DIR_ENT_REQUEST:
            case SPFS_CHANGE_DIR_ENT_REQUEST:
            case SPFS_TRUNCATE_REQUEST:
            case SPFS_MAKE_DIR_REQUEST:
            default:
                cerr << "BMI Server not yet implemented for "
                     << " name: " << msg->name()
                     << " kind: " << msg->kind()
                     << " info: " << msg->info() << endl;
                break;
        }

        // Encapsulate the file system response and send it to the client
        BMIServerSendMessage* pkt = new BMIServerSendMessage();
        pkt->encapsulate(response);
        pkt->setByteLength(256);
        pkt->setUniqueId(ev.getUniqueNumber());

        TCPSocket* responseSocket =
            static_cast<TCPSocket*>(msg->contextPointer());
        responseSocket->send(pkt);
        delete msg;
    }
}

void BMITcpServer::socketDataArrived(int, void *, cMessage *msg, bool)
{    
    // TCP settings should be set to tcp.sendQueueClass="TCPMsgBasedSendQueue"
    // and tcp.receiveQueueClass="TCPMsgBasedRcvQueue" ensuring that only
    // whole messages are received rather than message fragments

    // Find the socket for this message
    TCPSocket* responseSocket = socketMap_.findSocketFor(msg);
    
    // Decapsulate the payload and call handleMessage with the payload
    cMessage* payload = msg->decapsulate();

    // Store the response socket in the context pointer -- this is probably
    // not a good way to do this in the long run
    payload->setContextPointer(responseSocket);
    delete msg;
    handleMessage(payload);
}


/*
 * Local variables:
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 *
 * vim: ts=8 sts=4 sw=4 expandtab
 */
