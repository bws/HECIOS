//
// This file is part of Hecios
//
// Copyright (C) 2007 Brad Settlemyer
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
//
#include <cassert>
#include <iostream>
#include <map>
#include "TCPSocket.h"
#include "TCPSocketMap.h"
#include "bmi_endpoint.h"
#include "bmi_proto_m.h"
#include "ip_socket_map.h"
#include "pfs_utils.h"
#include "pvfs_proto_m.h"
#include <omnetpp.h>
using namespace std;

/**
 * Model of BMI Endpoint using TCP sockets as a transport
 */
class BMITcpEndpoint : public BMIEndpoint, public TCPSocket::CallbackInterface
{
public:
    /** @return a BMIExpected message encapsulating msg */
    virtual spfsBMIExpectedMessage* createExpectedMessage(cMessage* msg);
    
    /** @return a BMIUnexpected message encapsulating msg */
    virtual spfsBMIUnexpectedMessage* createUnexpectedMessage(
        spfsRequest* request);
    
protected:
    /** Implementation of cSimpleModule::handleMessage */
    virtual void handleMessage(cMessage* msg);

    /** Initialize the BMI TCP Endpoint */
    virtual void initializeEndpoint();

    /** Finalize the BMI TCP Endpoint */
    virtual void finalizeEndpoint();

    /** Send a BMIExpected message over the network */
    virtual void sendOverNetwork(spfsBMIExpectedMessage* expectedMsg);
    
    /** Send a BMIExpected message over the network */
    virtual void sendOverNetwork(spfsBMIUnexpectedMessage* unexpectedMsg);
    
    /** Extract the domain message from a BMI message */
    virtual cMessage* extractBMIPayload(spfsBMIMessage* bmiMsg);

    /** Implementation of TCPSocket::CallbackInterface::socketDataArrived */
    virtual void socketDataArrived(int connId, void* ptr, cMessage* msg,
                                   bool urgent);
    
    /** Handle the arrival of status messages to note when a msg is recvd */
    void socketStatusArrived(int connId, void *yourPtr, TCPStatusInfo *status);

private:
    /**
     * @return a socket with an open connection to the server that hosts handle
     */
    TCPSocket* getConnectedSocket(const FSHandle& handle);

    /** The port to listen on */
    int serverListenPort_;

    /** The port to connect to remote servers on */
    int clientConnectPort_;

    /** Gate id for tcpIn */
    int tcpInGateId_;

    /** Gate id for tcpOut */
    int tcpOutGateId_;

    /** Map for handling messages from open sockets */
    TCPSocketMap socketMap_;

    /** The server's listening socket */
    TCPSocket listenSocket_;

    /** Mapping of Server IP's to connected server sockets */
    IPSocketMap serverConnectionMap_;

    /** Mapping from a messages socketId to socket used to send it */
    std::map<int,TCPSocket*> requestToSocketMap_;
};

// OMNet Registriation Method
Define_Module(BMITcpEndpoint);

void BMITcpEndpoint::initializeEndpoint()
{
    // Extract the gate ids
    tcpInGateId_ = findGate("tcpIn");
    tcpOutGateId_ = findGate("tcpOut");

    // Extract the port information
    clientConnectPort_ = par("connectPort").longValue();
    serverListenPort_ = par("listenPort").longValue();
    assert(0 <= clientConnectPort_);
    assert(0 <= serverListenPort_);
    
    // Disable the listen socket if the port is set to 0 
    if (0 != serverListenPort_)
    {
        // Setup the socket receive stuff
        listenSocket_.setOutputGate(gate("tcpOut"));
        listenSocket_.setCallbackObject(this, 0);
    
        // Open the server side socket
        listenSocket_.bind(serverListenPort_);
        listenSocket_.listen();
    }
}

void BMITcpEndpoint::finalizeEndpoint()
{
    socketMap_.deleteSockets();
}

/**
 * Handle MPI-IO Response messages
 */
void BMITcpEndpoint::handleMessage(cMessage* msg)
{
    // Attempt to treat the message as a TCP message
    // Otherwise let the BMIEndpoint handle it
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

spfsBMIUnexpectedMessage* BMITcpEndpoint::createUnexpectedMessage(
    spfsRequest* request)
{
    assert(0 != request);
    spfsBMIUnexpectedMessage* pkt = new spfsBMIUnexpectedMessage();
    pkt->setHandle(request->getHandle());
    pkt->encapsulate(request);
    pkt->addByteLength(BMI_UNEXPECTED_MSG_BYTES);
    return pkt;
}

spfsBMIExpectedMessage* BMITcpEndpoint::createExpectedMessage(
    cMessage* msg)
{
    assert(0 != msg);

    // Retrieve the connection id used for the originating request
    spfsRequest* req = static_cast<spfsRequest*>(msg->contextPointer());
    
    spfsBMIExpectedMessage* pkt = new spfsBMIExpectedMessage();
    pkt->setConnectionId(req->getBmiConnectionId());
    pkt->encapsulate(msg);
    pkt->addByteLength(BMI_EXPECTED_MSG_BYTES);
    return pkt;
}

void BMITcpEndpoint::sendOverNetwork(spfsBMIExpectedMessage* msg)
{
    assert(0 !=msg);
    
    // Find the already open socket
    map<int,TCPSocket*>::iterator pos =
        requestToSocketMap_.find(msg->getConnectionId());
    assert(requestToSocketMap_.end() != pos);
        
    TCPSocket* responseSocket = pos->second;
    assert(0 != responseSocket);

    // Remove the entry for this socket
    // TODO: how to do map cleanup here
    //requestToSocketMap_.erase(req->getBmiConnectionId());
    responseSocket->send(msg);
}

void BMITcpEndpoint::sendOverNetwork(spfsBMIUnexpectedMessage* msg)
{
    assert(0 != msg);
    
    // Retrieve the socket for this handle
    TCPSocket* sock = getConnectedSocket(msg->getHandle());
    sock->send(msg);
}

cMessage* BMITcpEndpoint::extractBMIPayload(spfsBMIMessage* bmiMsg)
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

//
// TCP settings should be set to tcp.sendQueueClass="TCPMsgBasedSendQueue"
// and tcp.receiveQueueClass="TCPMsgBasedRcvQueue" ensuring that only
// whole messages are received rather than message fragments
//
void BMITcpEndpoint::socketDataArrived(int, void *, cMessage *msg, bool)
{
    BMIEndpoint::handleMessage(msg);
}

//
// Implemented because at one point I was trying to intercept this call
// Probably no longer needed, as it isn't called
//
void BMITcpEndpoint::socketStatusArrived(int connId,
                                       void *yourPtr,
                                       TCPStatusInfo* status)
{
    cerr << "Socket status arrived: State: " << status->state()
         << " Name: " << status->stateName()
         << " MSS: " << status->snd_mss()
         << " Final Ack: " << status->fin_ack_rcvd() << endl;
    delete status;
}

TCPSocket* BMITcpEndpoint::getConnectedSocket(const FSHandle& handle)
{
    assert(0 != clientConnectPort_);
    IPvXAddress* serverIp = PFSUtils::instance().getServerIP(handle);
    TCPSocket* sock = serverConnectionMap_.getSocket(serverIp->str());

    // If a connected socket does not exist, create it
    if (0 == sock)
    {
        sock = new TCPSocket();
        sock->setOutputGate(gate("tcpOut"));
        sock->setCallbackObject(this, NULL);
        sock->connect(serverIp->get4(), clientConnectPort_);

        // Add open socket for use in later communication
        serverConnectionMap_.addSocket(serverIp->str(), sock);

        // Add open socket to TCPSocketMap for handling later TCP messages
        socketMap_.addSocket(sock);
    }

    return sock;
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
