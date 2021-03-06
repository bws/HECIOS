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
#include "TCPSocket.h"
#include "TCPSocketMap.h"
#include "basic_types.h"
#include "bmi_endpoint.h"
#include "bmi_proto_m.h"
#include "ip_socket_map.h"
#include "pfs_utils.h"
#include "pvfs_proto_m.h"
#include "umd_io_trace.h"
#include <omnetpp.h>
using namespace std;

/**
 * Model of a file system client library.
 */
class BMITcpServer : public BMIEndpoint, public TCPSocket::CallbackInterface
{
public:
    /** @return a BMIExpected message encapsulating msg */
    virtual spfsBMIExpectedMessage* createExpectedMessage(cPacket* msg);

    /** @return a BMIUnexpected message encapsulating msg */
    virtual spfsBMIUnexpectedMessage* createUnexpectedMessage(
        spfsRequest* request);

protected:
    /** Implementation of initialize */
    virtual void initializeEndpoint();

    /** Implementation of finish */
    virtual void finalizeEndpoint();

    /** Implementation of cSimpleModule::handleMessage */
    virtual void handleMessage(cMessage* msg);

    /** Send a BMIExpected message over the network */
    virtual void sendOverNetwork(spfsBMIExpectedMessage* expectedMsg);

    /** Send a BMIExpected message over the network */
    virtual void sendOverNetwork(spfsBMIUnexpectedMessage* unexpectedMsg);

    /** Extract the domain message from a BMI message */
    virtual cMessage* extractBMIPayload(spfsBMIMessage* bmiMsg);

    /** Implementation of TCPSocket::CallbackInterface::socketDataArrived */
    virtual void socketDataArrived(int connId, void* ptr, cPacket* msg,
                                   bool urgent);

    /** Handle the arrival of a socket failure message */
    virtual void socketFailure(int connId, void *yourPtr, int code);

private:
    /** @return a socket with an open connection to a remote server */
    TCPSocket* getConnectedSocket(const FSHandle& handle);

    /** Gate id for tcpIn */
    int tcpInGateId_;

    /** Gate id for tcpOut */
    int tcpOutGateId_;

    /** The port to listen on */
    int listenPort_;

    /** The server's listening socket */
    TCPSocket listenSocket_;

    /** Map for handling messages from open client sockets */
    TCPSocketMap socketMap_;

    /** Mapping from a messages connectionId to the socket */
    std::map<ConnectionId,TCPSocket*> requestToSocketMap_;

    /** Mapping of Server IP's to connected server sockets */
    IPSocketMap remoteServerConnectionMap_;
};

// OMNet Registriation Method
Define_Module(BMITcpServer);

void BMITcpServer::initializeEndpoint()
{
    // Extract the gate ids
    tcpInGateId_ = findGate("tcpIn");
    tcpOutGateId_ = findGate("tcpOut");

    // Extract the port information
    listenPort_ = par("listenPort").longValue();
    assert (0 <= listenPort_);

    // Setup the socket receive stuff
    listenSocket_.setOutputGate(gate("tcpOut"));
    listenSocket_.setCallbackObject(this, 0);

    // Open the server side socket
    listenSocket_.bind(listenPort_);
    listenSocket_.listen();
}

void BMITcpServer::finalizeEndpoint()
{
    socketMap_.deleteSockets();
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
        BMIEndpoint::handleMessage(msg);
    }
}

spfsBMIUnexpectedMessage* BMITcpServer::createUnexpectedMessage(
    spfsRequest* request)
{
    //cerr << __FILE__ << ":" << __LINE__ << ":"
    //     << "Unexpected message creation by server" << endl;
    assert(0 != request);
    spfsBMIUnexpectedMessage* pkt = new spfsBMIUnexpectedMessage();
    pkt->setHandle(request->getHandle());
    pkt->encapsulate(request);
    pkt->addByteLength(BMI_UNEXPECTED_MSG_BYTES);
    return pkt;
}

spfsBMIExpectedMessage* BMITcpServer::createExpectedMessage(
    cPacket* msg)
{
    assert(0 != msg);

    // Retrieve the connection id used for the originating request
    spfsRequest* req = static_cast<spfsRequest*>(msg->getContextPointer());

    spfsBMIExpectedMessage* pkt = new spfsBMIExpectedMessage();
    pkt->setConnectionId(req->getBmiConnectionId());
    pkt->encapsulate(msg);

    // Add the send overhead
    msg->addByteLength(BMI_EXPECTED_MSG_BYTES);
    return pkt;
}

void BMITcpServer::sendOverNetwork(spfsBMIExpectedMessage* msg)
{
    assert(0 != msg);
    assert(0 < msg->getByteLength());

    // Find the already open socket
    map<ConnectionId,TCPSocket*>::iterator pos =
        requestToSocketMap_.find(msg->getConnectionId());
    assert(requestToSocketMap_.end() != pos);

    TCPSocket* responseSocket = pos->second;
    assert(0 != responseSocket);

    // Remove the entry for this socket
    // FIXME: how to do map cleanup here
    //requestToSocketMap_.erase(req->getBmiConnectionId());
    responseSocket->send(msg);
}

void BMITcpServer::sendOverNetwork(spfsBMIUnexpectedMessage* msg)
{
    assert(0 != msg);
    assert(0 < msg->getByteLength());
    //cerr << __FILE__ << ":" << __LINE__ << ":"
    //     << "Server sending unexpected messages" << endl;

    // Retrieve the socket for this handle
    TCPSocket* sock = getConnectedSocket(msg->getHandle());
    sock->send(msg);
}

cMessage* BMITcpServer::extractBMIPayload(spfsBMIMessage* bmiMsg)
{
    assert(0 != bmiMsg);

    // Find the socket for this message
    TCPSocket* responseSocket = socketMap_.findSocketFor(bmiMsg);

    // Decapsulate the payload and call handleMessage with the payload
    cMessage* payload = bmiMsg->decapsulate();
    assert(0 != payload);

    // If this is a request, store the socket for use during the response
    if (spfsRequest* request = dynamic_cast<spfsRequest*>(payload))
    {
        static int nextSocketId = 0;
        request->setBmiConnectionId(nextSocketId++);
        requestToSocketMap_[request->getBmiConnectionId()] = responseSocket;
    }
    return payload;
}

TCPSocket* BMITcpServer::getConnectedSocket(const FSHandle& handle)
{
    IPvXAddress* serverIp = PFSUtils::instance().getServerIP(handle);
    TCPSocket* sock = remoteServerConnectionMap_.getSocket(serverIp->str(),
                                                           listenPort_);

    // If a connected socket does not exist, create it
    if (0 == sock)
    {
        sock = new TCPSocket();
        sock->setOutputGate(gate("tcpOut"));
        sock->setCallbackObject(this, NULL);
        sock->connect(serverIp->get4(), listenPort_);

        // Add open socket for use in later communication
        remoteServerConnectionMap_.addSocket(serverIp->str(),
                                             listenPort_,
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
void BMITcpServer::socketDataArrived(int, void *, cPacket *msg, bool)
{
    BMIEndpoint::handleMessage(msg);
}

void BMITcpServer::socketFailure(int connId, void *yourPtr, int code)
{
    cerr << __FILE__ << ":" << __LINE__ << ":"
         << "Socket Failure: ConnId: " << connId
         << " Code: " << code << endl;
}

/*
 * Local variables:
 *  indent-tabs-mode: nil
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 *
 * vim: ts=8 sts=4 sw=4 expandtab
 */
