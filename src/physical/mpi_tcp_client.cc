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
#include "IPvXAddress.h"
#include "TCPSocket.h"
#include "TCPSocketMap.h"
#include "ip_socket_map.h"
#include "mpi_proto_m.h"
#include "network_proto_m.h"
#include "pfs_utils.h"
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
}

/**
 * Handle MPI Response messages
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
    else if (spfsMPISendRequest* req = dynamic_cast<spfsMPISendRequest*>(msg))
    {
        // Retrieve the socket for this rank
        TCPSocket* sock = getConnectedSocket(req->getRank());
                
        // Encapsulate the domain message and send via TCP
        spfsNetworkClientSendMessage* pkt = new spfsNetworkClientSendMessage();
        pkt->encapsulate(msg);
        pkt->setByteLength(256);
        sock->send(pkt);
    }
    else if (0 != dynamic_cast<spfsMPISendResponse*>(msg))
    {
        // Send response to application
        send(msg, appOutGateId_);
    }
    else
    {
        cerr << "Unsupported type in MPI TCP client\n";
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
