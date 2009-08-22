//
// This file is part of Hecios
//
// Copyright (C) 2007,2008,2009 Brad Settlemyer
//
// This file is distributed WITHOUT ANY WARRANTY. See the file 'License.txt'
// for details on this and other legal matters.
//
#include <cassert>
#include <iostream>
#include <map>
#include <omnetpp.h>
#include "TCPSocket.h"
#include "TCPSocketMap.h"
#include "cache_proto_m.h"
#include "ip_socket_map.h"
#include "mpi_proto_m.h"
#include "network_proto_m.h"
#include "pfs_utils.h"
using namespace std;

/**
 * Model of an MPI network server connection
 */
class MPITcpServer : public cSimpleModule, public TCPSocket::CallbackInterface
{
public:
    /** Constructor */
    MPITcpServer() : cSimpleModule() {};

protected:
    /** Implementation of initialize */
    virtual void initialize(int stage);

    /** Implementation of finish */
    virtual void finish() {};

    /** Implementation of cSimpleModule::handleMessage */
    virtual void handleMessage(cMessage* msg);

    virtual void handleApplicationMessage(cMessage* msg);

    /** Implementation of TCPSocket::CallbackInterface::socketDataArrived */
    virtual void socketDataArrived(int connId, void* ptr, cPacket* msg,
                                   bool urgent);

    /** Handle the arrival of a socket failure message */
    virtual void socketFailure(int connId, void *yourPtr, int code);

private:
    /** Send a message over an already established connection */
    void sendExpectedMessage(const ConnectionId& connId, cPacket* payload);

    /** Send a message over an unestablished connection */
    void sendUnexpectedMessage(int rank, cPacket* payload);

    /** @return a socket with an open connection to a remote server */
    TCPSocket* getConnectedSocket(int rank);

    /** Gate id for appIn */
    int appInGateId_;

    /** Gate id for appOut */
    int appOutGateId_;

    /** Gate id for netIn */
    int netInGateId_;

    /** Gate id for netOut */
    int netOutGateId_;

    /** The port to listen on */
    int listenPort_;

    /** Map for handling messages from open sockets */
    TCPSocketMap socketMap_;

    /** The server's listening socket */
    TCPSocket listenSocket_;

    /** Mapping from a messages connectionId to the sending socket */
    std::map<ConnectionId, TCPSocket*> requestToSocketMap_;

    /** Mapping of Server IP's to connected server sockets */
    IPSocketMap remoteServerConnectionMap_;
};

// OMNet Registriation Method
Define_Module(MPITcpServer);

void MPITcpServer::initialize(int stage)
{
    if (0 == stage)
    {
        // Setup the socket receive stuff
        listenSocket_.setOutputGate(gate("tcpOut"));
        listenSocket_.setCallbackObject(this, 0);

        // Extract the gate ids
        appInGateId_ = findGate("appIn");
        appOutGateId_ = findGate("appOut");
        netInGateId_ = findGate("tcpIn");
        netOutGateId_ = findGate("tcpOut");
    }
    else if (4 == stage)
    {
        // Extract the port information
        listenPort_ = par("listenPort").longValue();

        // Open the server side socket
        cerr << "Attempting to listen on port: " << listenPort_ << endl;
        listenSocket_.bind(listenPort_);
        listenSocket_.listen();
    }
}

/**
 * Handle MPI Response messages
 */
void MPITcpServer::handleMessage(cMessage* msg)
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
    else if (msg->getArrivalGateId() == appInGateId_)
    {
        handleApplicationMessage(msg);
     }
    else if (msg->getArrivalGateId() == netInGateId_)
    {
        send(msg, appOutGateId_);
    }
    else
    {
        cerr << __FILE__ << ":" << __LINE__ << ":"
             << "MPI TCP Server does not support message type "
             << " name: " << msg->getName()
             << " kind: " << msg->getKind()
             << " info: " << msg->info() << endl;
        assert(false);
    }
}

void MPITcpServer::handleApplicationMessage(cMessage* msg)
{
    // Determine if this is a request or a response to figure out
    // the type of socket to use
    if (spfsMPIRequest* req = dynamic_cast<spfsMPIRequest*>(msg))
    {
        assert(req == 0);
        assert(false);
    }
    else if (spfsCacheRequest* req = dynamic_cast<spfsCacheRequest*>(msg))
    {
        int destRank = req->getDestinationRank();
        sendUnexpectedMessage(destRank, req);
    }
    else if (spfsMPIResponse* req = dynamic_cast<spfsMPIResponse*>(msg))
    {
        spfsMPIRequest* origReq =
                static_cast<spfsMPIRequest*>(req->getContextPointer());
        ConnectionId connId = origReq->getConnectionId();
        sendExpectedMessage(connId, req);
    }
    else if (spfsCacheResponse* resp = dynamic_cast<spfsCacheResponse*>(msg))
    {
        spfsCacheRequest* origReq =
                static_cast<spfsCacheRequest*>(resp->getContextPointer());
        ConnectionId connId = origReq->getMpiConnectionId();
        sendExpectedMessage(connId, resp);
    }
    else
    {
        cerr << __FILE__ << ":" << __LINE__ << ":"
             << "Unsupported application message type!" << endl;
        assert(false);
    }
}

void MPITcpServer::sendExpectedMessage(const ConnectionId& connId,
                                       cPacket* payload)
{
    map<ConnectionId, TCPSocket*>::iterator pos = requestToSocketMap_.find(connId);
    assert(requestToSocketMap_.end() != pos);

    TCPSocket* responseSocket = pos->second;
    assert(0 != responseSocket);

    // Encapsulate the message and send to the peer
    spfsNetworkServerSendMessage* pkt = new spfsNetworkServerSendMessage();
    pkt->encapsulate(payload);
    pkt->setByteLength(4);
    responseSocket->send(pkt);

    // Remove the entry for this socket
    requestToSocketMap_.erase(connId);
}

void MPITcpServer::sendUnexpectedMessage(int rank, cPacket* payload)
{
    TCPSocket* socket = getConnectedSocket(rank);

    // Encapsulate the message and send to the peer
    spfsNetworkServerSendMessage* pkt = new spfsNetworkServerSendMessage();
    pkt->encapsulate(payload);
    pkt->setByteLength(4);
    socket->send(pkt);
}


TCPSocket* MPITcpServer::getConnectedSocket(int rank)
{
    PFSUtils::ConnectionDescriptor cd =
        PFSUtils::instance().getRankConnectionDescriptor(rank);
    IPvXAddress* serverIp = cd.first;
    size_t connectPort = cd.second;
    TCPSocket* sock = remoteServerConnectionMap_.getSocket(serverIp->str(),
                                                           connectPort);

    // If a connected socket does not exist, create it
    if (0 == sock)
    {
        sock = new TCPSocket();
        sock->setOutputGate(gate("tcpOut"));
        sock->setCallbackObject(this, NULL);
        sock->connect(serverIp->get4(), listenPort_);

        // Add open socket for use in later communication
        remoteServerConnectionMap_.addSocket(serverIp->str(),
                                             connectPort,
                                             sock);

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
void MPITcpServer::socketDataArrived(int, void *, cPacket* msg, bool)
{
    // Find the socket for this message
    TCPSocket* responseSocket = socketMap_.findSocketFor(msg);

    // Decapsulate the payload and call handleMessage with the payload
    cMessage* payload = msg->decapsulate();

    // Store the response socket for use during response
    static int nextSocketId = 0;
    if (spfsCacheRequest* request = dynamic_cast<spfsCacheRequest*>(payload))
    {
        request->setMpiConnectionId(nextSocketId++);
    }
    else if (spfsMPIRequest* request = dynamic_cast<spfsMPIRequest*>(payload))
    {
        request->setConnectionId(nextSocketId++);
    }
    else
    {
        cerr << __FILE__ << ":" << __LINE__ << ":"
             << "Error: Invalid MPI Payload: " << msg->info() << endl;
        assert(false);
    }
    requestToSocketMap_[nextSocketId - 1] = responseSocket;

    delete msg;
    handleMessage(payload);
}

void MPITcpServer::socketFailure(int connId, void *yourPtr, int code)
{
    cerr << __FILE__ << ":" << __LINE__ << ":"
         << "Socket Failure: ConnId: " << connId
         << " Code: " << code << endl;
    assert(false);
}

/*
 * Local variables:
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 *
 * vim: ts=8 sts=4 sw=4 expandtab
 */
