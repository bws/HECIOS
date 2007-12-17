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
    /** Number of bytes required as overhead to use the TCP protocol */
    static const unsigned int TCP_OVERHEAD_BYTES = 4;
    
    /** @return a BMIExpected message encapsulating msg */
    virtual spfsBMIExpectedMessage* createExpectedMessage(cMessage* msg);
    
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
    virtual void socketDataArrived(int connId, void* ptr, cMessage* msg,
                                   bool urgent);
    
private:

    /** The port to listen on */
    int listenPort_;

    /** Map for handling messages from open sockets */
    TCPSocketMap socketMap_;

    /** The server's listening socket */
    TCPSocket listenSocket_;

    /** Mapping from a messages socketId to the socket */
    std::map<int,TCPSocket*> requestToSocketMap_;

    /** Gate id for appIn */
    //int appInGateId_;

    /** Gate id for appOut */
    //int appOutGateId_;

    /** Gate id for tcpIn */
    int tcpInGateId_;
    
    /** Gate id for tcpOut */
    int tcpOutGateId_;
};

// OMNet Registriation Method
Define_Module(BMITcpServer);

void BMITcpServer::initializeEndpoint()
{
    // Extract the port information
    listenPort_ = par("listenPort").longValue();

    // Setup the socket receive stuff
    listenSocket_.setOutputGate(gate("tcpOut"));
    listenSocket_.setCallbackObject(this, 0);
    
    // Open the server side socket
    listenSocket_.bind(listenPort_);
    listenSocket_.listen();

    // Extract the gate ids
    //appInGateId_ = gate("appIn")->id();
    //appOutGateId_ = gate("appOut")->id();
    tcpInGateId_ = gate("tcpIn")->id();
    tcpOutGateId_ = gate("tcpOut")->id();
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
    cerr << "Unexpected message creation not supported on server" << endl; 
    return 0;
}

spfsBMIExpectedMessage* BMITcpServer::createExpectedMessage(
    cMessage* msg)
{
    assert(0 != msg);

    // Retrieve the connection id used for the originating request
    spfsRequest* req = static_cast<spfsRequest*>(msg->contextPointer());
    
    spfsBMIExpectedMessage* pkt = new spfsBMIExpectedMessage();
    pkt->setConnectionId(req->getBmiConnectionId());
    pkt->encapsulate(msg);
    return pkt;
}

void BMITcpServer::sendOverNetwork(spfsBMIExpectedMessage* msg)
{
    assert(0 != msg);
    
    // Find the already open socket
    map<int,TCPSocket*>::iterator pos =
        requestToSocketMap_.find(msg->getConnectionId());
    assert(requestToSocketMap_.end() != pos);
        
    TCPSocket* responseSocket = pos->second;
    assert(0 != responseSocket);

    // Add the send overhead
    msg->addByteLength(BMI_OVERHEAD_BYTES + TCP_OVERHEAD_BYTES);
    
    // Remove the entry for this socket
    // FIXME: how to do map cleanup here
    //requestToSocketMap_.erase(req->getBmiConnectionId());
    responseSocket->send(msg);
}

void BMITcpServer::sendOverNetwork(spfsBMIUnexpectedMessage* msg)
{
    cerr << __FILE__ << ":" << __LINE__ << ":"
         << "Server does not support sending unexpected messages" << endl;
    assert(false);
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

//
// TCP settings should be set to tcp.sendQueueClass="TCPMsgBasedSendQueue"
// and tcp.receiveQueueClass="TCPMsgBasedRcvQueue" ensuring that only
// whole messages are received rather than message fragments
//
void BMITcpServer::socketDataArrived(int, void *, cMessage *msg, bool)
{
    BMIEndpoint::handleMessage(msg);
}


/*
 * Local variables:
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 *
 * vim: ts=8 sts=4 sw=4 expandtab
 */
