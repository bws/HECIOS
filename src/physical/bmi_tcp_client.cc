#include <cassert>
#include <iostream>
#include "IPvXAddress.h"
#include "TCPSocket.h"
#include "TCPSocketMap.h"
#include "bmi_proto_m.h"
#include "mpiio_proto_m.h"
#include "pfs_types.h"
#include "pfs_utils.h"
#include "pvfs_proto_m.h"
#include "ip_socket_map.h"
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
    TCPSocket* getConnectedSocket(const FSHandle& handle);

    /** The server port to connect to */
    int connectPort_;

    /** Mapping of Server IP's to connected server sockets */
    IPSocketMap serverConnectionMap_;

    /** Map containing all connected sockets (for message handling) */
    TCPSocketMap socketMap_;

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
    // Determine if the msg originated from TCP
    if (TCPSocket::belongsToAnyTCPSocket(msg))
    {
        TCPSocket* sock = socketMap_.findSocketFor(msg);
        assert(0 != sock);
        sock->processMessage(msg);
    }
    else
    {
        // Send requests to the server via TCP
        switch(msg->kind())
        {
            case SPFS_MPI_FILE_OPEN_REQUEST:
            case SPFS_MPI_FILE_CLOSE_REQUEST:
            case SPFS_MPI_FILE_DELETE_REQUEST:
            case SPFS_MPI_FILE_SET_SIZE_REQUEST:
            case SPFS_MPI_FILE_PREALLOCATE_REQUEST:
            case SPFS_MPI_FILE_GET_SIZE_REQUEST:
            case SPFS_MPI_FILE_GET_INFO_REQUEST:
            case SPFS_MPI_FILE_SET_INFO_REQUEST:
            case SPFS_MPI_FILE_READ_AT_REQUEST:
            case SPFS_MPI_FILE_READ_REQUEST:
            case SPFS_MPI_FILE_WRITE_AT_REQUEST:
            case SPFS_MPI_FILE_WRITE_REQUEST:
            case SPFS_LOOKUP_PATH_REQUEST:
            {
                cerr << "Sending packet from client!"
                     << "Kind: " << msg->kind() << " info: " << msg->info()
                     << endl;
                
                // Retrieve the socket for this handle
                FSHandle handle = 0;
                TCPSocket* sock = getConnectedSocket(handle);
                
                // Encapsulate the domain message and send via TCP
                spfsBMIClientSendMessage* pkt = new spfsBMIClientSendMessage();
                pkt->encapsulate(msg);
                pkt->setByteLength(256);
                pkt->setUniqueId(ev.getUniqueNumber());
                sock->send(pkt);
                break;
            }
            case SPFS_MPI_FILE_OPEN_RESPONSE:
            case SPFS_MPI_FILE_CLOSE_RESPONSE:
            case SPFS_MPI_FILE_DELETE_RESPONSE:
            case SPFS_MPI_FILE_SET_SIZE_RESPONSE:
            case SPFS_MPI_FILE_PREALLOCATE_RESPONSE:
            case SPFS_MPI_FILE_GET_SIZE_RESPONSE:
            case SPFS_MPI_FILE_GET_INFO_RESPONSE:
            case SPFS_MPI_FILE_SET_INFO_RESPONSE:
            case SPFS_MPI_FILE_READ_AT_RESPONSE:
            case SPFS_MPI_FILE_READ_RESPONSE:
            case SPFS_MPI_FILE_WRITE_AT_RESPONSE:
            case SPFS_MPI_FILE_WRITE_RESPONSE:
            {
                // Send response to application
                send(msg, "bmiOut");
            }
            default:
                cerr << "Unknown message type in BMI client\n";
        }
    }
}

TCPSocket* BMITcpClient::getConnectedSocket(const FSHandle& handle)
{
    IPvXAddress serverIp = PFSUtils::instance().getServerIP(handle);
    TCPSocket* sock = serverConnectionMap_.getSocket(serverIp.str());

    // If a connected socket does not exist, create it
    if (0 == sock)
    {
        sock = new TCPSocket();
        sock->setOutputGate(gate("tcpOut"));
        sock->setCallbackObject(this, NULL);
        sock->connect(serverIp, connectPort_);

        // Add open socket for use in later communication
        serverConnectionMap_.addSocket(serverIp.str(), sock);

        // Add open socket to TCPSocketMap for handling later TCP messages
        socketMap_.addSocket(sock);
    }

    return sock;
}

void BMITcpClient::socketDataArrived(int, void *, cMessage *msg, bool)
{
    // TCP settings should be set to tcp.sendQueueClass="TCPMsgBasedSendQueue"
    // and tcp.receiveQueueClass="TCPMsgBasedRcvQueue" ensuring that only
    // whole messages are received rather than message fragments

    // Decapsulate the payload and call handleMessage with the payload
    cMessage* payload = msg->decapsulate();
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
