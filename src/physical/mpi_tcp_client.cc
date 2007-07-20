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
 * Model of a MPI network client connection
 */
class MPITcpClient : public cSimpleModule, public TCPSocket::CallbackInterface
{
public:
    /** Constructor */
    MPITcpClient() : cSimpleModule() {};
    
protected:
    /** Implementation of initialize */
    virtual void initialize();

    /** Implementation of finish */
    virtual void finish() {};

    /** Implementation of handleMessage */
    virtual void handleMessage(cMessage* msg);

    /** Extract the payload from a completed socket message */
    void socketDataArrived(int, void *, cMessage *msg, bool);
    
private:

    /** @return a socket with an open connection to the server */
    TCPSocket* getConnectedSocket(int rank);

    /** The server port to connect to */
    int connectPort_;

    /** Mapping of Server IP's to connected server sockets */
    IPSocketMap serverConnectionMap_;

    /** Map containing all connected sockets (for message handling) */
    TCPSocketMap socketMap_;

    /** Gate id for appIn */
    int appOutGateId_;

    /** Gate id for tcpIn */
    int tcpInGateId_;
};

// OMNet Registriation Method
Define_Module(MPITcpClient);

/**
 *
 */
void MPITcpClient::initialize()
{
    connectPort_ = par("connectPort").longValue();
    appOutGateId_ = gate("appOut")->id();
    tcpInGateId_ = gate("tcpIn")->id();
}

/**
 * Handle MPI-IO Response messages
 */
void MPITcpClient::handleMessage(cMessage* msg)
{
    // Determine if the msg originated from TCP
    if (TCPSocket::belongsToAnyTCPSocket(msg))
    {
        TCPSocket* sock = socketMap_.findSocketFor(msg);
        assert(0 != sock);
        sock->processMessage(msg);
    }
    else if (spfsRequest* req = dynamic_cast<spfsRequest*>(msg))
    {
        // Retrieve the socket for this handle
        FSHandle handle = req->getHandle();
        TCPSocket* sock = getConnectedSocket(handle);
                
        // Encapsulate the domain message and send via TCP
        spfsBMIClientSendMessage* pkt = new spfsBMIClientSendMessage();
        pkt->encapsulate(msg);
        pkt->setByteLength(256);
        pkt->setUniqueId(ev.getUniqueNumber());
        sock->send(pkt);

        // A mostly ineffective hack to disable excessive INET output
        // ev.disable_tracing = true;
    }
    else if (0 != dynamic_cast<spfsResponse*>(msg))
    {
        // Send response to application
        send(msg, appOutGateId_);
                
        // A mostly ineffective hack to disable excessive INET output
        // ev.disable_tracing = false;
    }
    else
    {
        cerr << "Unknown message type in MPI TCP client\n";
    }
}

TCPSocket* MPITcpClient::getConnectedSocket(int rank)
{
    IPvXAddress* ip = PFSUtils::instance().getRankIP(rank);
    TCPSocket* sock = serverConnectionMap_.getSocket(ip->str());

    // If a connected socket does not exist, create it
    if (0 == sock)
    {
        sock = new TCPSocket();
        sock->setOutputGate(gate("tcpOut"));
        sock->setCallbackObject(this, NULL);
        sock->connect(ip->get4(), connectPort_);

        // Add open socket for use in later communication
        serverConnectionMap_.addSocket(ip->str(), sock);

        // Add open socket to TCPSocketMap for handling later TCP messages
        socketMap_.addSocket(sock);
    }

    return sock;
}

//
// TCP settings should be set to tcp.sendQueueClass="TCPMsgBasedSendQueue"
// and tcp.receiveQueueClass="TCPMsgBasedRcvQueue" ensuring that only
// whole messages are received rather than message fragments
//
void MPITcpClient::socketDataArrived(int, void *, cMessage *msg, bool)
{
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
