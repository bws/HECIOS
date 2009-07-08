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
#include "IPvXAddress.h"
#include "TCPCommand_m.h"
#include "TCPSocket.h"
#include "TCPSocketMap.h"
#include "basic_types.h"
#include "bmi_endpoint.h"
#include "bmi_proto_m.h"
#include "ip_socket_map.h"
#include "pfs_utils.h"
#include "pvfs_proto_m.h"
#include <omnetpp.h>
using namespace std;

/**
 * Model of a file system client library.
 */
class BMITcpClient : public BMIEndpoint, public TCPSocket::CallbackInterface
{
public:

    /** Constructor */
    BMITcpClient() : BMIEndpoint() {};

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

    /** Implementation of handleMessage */
    virtual void handleMessage(cMessage* msg);

    /** Send a BMIExpected message over the network */
    virtual void sendOverNetwork(spfsBMIExpectedMessage* expectedMsg);

    /** Send a BMIExpected message over the network */
    virtual void sendOverNetwork(spfsBMIUnexpectedMessage* unexpectedMsg);

    /** Extract the payload from a completed socket message */
    virtual void socketDataArrived(int, void *, cMessage *msg, bool);

    /** Handle the arrival of a socket failure message */
    virtual void socketFailure(int connId, void *yourPtr, int code);

    /** Handle the arrival of status messages to note when a msg is recvd */
    virtual void socketStatusArrived(int connId, void *yourPtr, TCPStatusInfo *status);

private:

    /** @return a socket with an open connection to the server */
    TCPSocket* getConnectedSocket(const FSHandle& handle);

    /** Gate id for tcpIn */
    int tcpInGateId_;

    /** Gate id for tcpOut */
    int tcpOutGateId_;

    /** The server port to connect to */
    int connectPort_;

    /** Mapping of Server IP's to connected server sockets */
    IPSocketMap serverConnectionMap_;

    /** Map containing all connected sockets (for message handling) */
    TCPSocketMap socketMap_;
};

// OMNet Registriation Method
Define_Module(BMITcpClient);

void BMITcpClient::initializeEndpoint()
{
    // Extract the gate ids
    tcpInGateId_ = findGate("tcpIn");
    tcpOutGateId_ = findGate("tcpOut");

    // Extract the port information
    connectPort_ = par("connectPort").longValue();
    assert(0 <= connectPort_);
}

void BMITcpClient::finalizeEndpoint()
{
    socketMap_.deleteSockets();
}

/**
 * Handle TCP messages
 */
void BMITcpClient::handleMessage(cMessage* msg)
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

spfsBMIUnexpectedMessage* BMITcpClient::createUnexpectedMessage(
    spfsRequest* request)
{
    assert(0 != request);
    spfsBMIUnexpectedMessage* pkt = new spfsBMIUnexpectedMessage();
    pkt->setHandle(request->getHandle());
    pkt->encapsulate(request);
    pkt->addByteLength(BMI_UNEXPECTED_MSG_BYTES);
    return pkt;
}

spfsBMIExpectedMessage* BMITcpClient::createExpectedMessage(cMessage* msg)
{
    assert(0 != msg);
    cerr << __FILE__ << ":" << __LINE__ << ":"
         << "TCPClient is unable to create expected messages" << endl;
    return 0;
}

void BMITcpClient::sendOverNetwork(spfsBMIExpectedMessage* msg)
{
    assert(0 != msg);
    assert(0 < msg->byteLength());

    // Retrieve the socket for this handle
    TCPSocket* sock = getConnectedSocket(msg->getConnectionId());
    sock->send(msg);
}

void BMITcpClient::sendOverNetwork(spfsBMIUnexpectedMessage* msg)
{
    assert(0 != msg);
    assert(0 < msg->byteLength());

    // Retrieve the socket for this handle
    TCPSocket* sock = getConnectedSocket(msg->getHandle());
    sock->send(msg);
}

TCPSocket* BMITcpClient::getConnectedSocket(const FSHandle& handle)
{
    IPvXAddress* serverIp = PFSUtils::instance().getServerIP(handle);
    TCPSocket* sock = serverConnectionMap_.getSocket(serverIp->str(),
                                                     connectPort_);

    // If a connected socket does not exist, create it
    if (0 == sock)
    {
        sock = new TCPSocket();
        sock->setOutputGate(gate("tcpOut"));
        sock->setCallbackObject(this, NULL);
        sock->connect(serverIp->get4(), connectPort_);

        // Add open socket for use in later communication
        serverConnectionMap_.addSocket(serverIp->str(), connectPort_, sock);

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
void BMITcpClient::socketDataArrived(int, void *, cMessage *msg, bool)
{
    BMIEndpoint::handleMessage(msg);
}

void BMITcpClient::socketFailure(int connId, void *yourPtr, int code)
{
    cerr << __FILE__ << ":" << __LINE__ << ":"
         << "Socket Failure: ConnId: " << connId
         << " Code: " << code << endl;
}

void BMITcpClient::socketStatusArrived(int connId,
                                       void *yourPtr,
                                       TCPStatusInfo* status)
{
    cerr << __FILE__ << ":" << __LINE__ << ":"
         << "Socket status arrived: State: " << status->state()
         << " Name: " << status->stateName()
         << " MSS: " << status->snd_mss()
         << " Final Ack: " << status->fin_ack_rcvd() << endl;
    delete status;
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
