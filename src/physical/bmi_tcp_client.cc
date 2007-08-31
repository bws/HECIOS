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
#include "TCPCommand_m.h"
#include "TCPSocket.h"
#include "TCPSocketMap.h"
#include "network_proto_m.h"
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

    /** Number of bytes required as overhead to use the BMI protocol */
    static const unsigned int OVERHEAD_BYTES = 4;
    
    /** Constructor */
    BMITcpClient() : cSimpleModule() {};
    
protected:
    /** Implementation of initialize */
    virtual void initialize();

    /** Implementation of finish */
    virtual void finish() {};

    /** Implementation of handleMessage */
    virtual void handleMessage(cMessage* msg);

    /** Extract the payload from a completed socket message */
    void socketDataArrived(int, void *, cMessage *msg, bool);

    /** Handle the arrival of status messages to note when a msg is recvd */
    void socketStatusArrived(int connId, void *yourPtr, TCPStatusInfo *status);
    
private:

    /** @return a socket with an open connection to the server */
    TCPSocket* getConnectedSocket(const FSHandle& handle);

    /** The server port to connect to */
    int connectPort_;

    /** Mapping of Server IP's to connected server sockets */
    IPSocketMap serverConnectionMap_;

    /** Map containing all connected sockets (for message handling) */
    TCPSocketMap socketMap_;

    /** Gate id for bmiIn */
    int bmiInGateId_;

    /** Gate id for tcpIn */
    int tcpInGateId_;
};

// OMNet Registriation Method
Define_Module(BMITcpClient);

/**
 *
 */
void BMITcpClient::initialize()
{
    connectPort_ = par("connectPort").longValue();
    bmiInGateId_ = gate("bmiIn")->id();
    tcpInGateId_ = gate("tcpIn")->id();
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
    else if (spfsRequest* req = dynamic_cast<spfsRequest*>(msg))
    {
        // Retrieve the socket for this handle
        FSHandle handle = req->getHandle();
        TCPSocket* sock = getConnectedSocket(handle);
                
        // Encapsulate the domain message and send via TCP
        spfsNetworkClientSendMessage* pkt = new spfsNetworkClientSendMessage();
        pkt->setByteLength(OVERHEAD_BYTES);
        pkt->encapsulate(msg);
        pkt->setUniqueId(ev.getUniqueNumber());
        sock->send(pkt);
    }
    else if (0 != dynamic_cast<spfsResponse*>(msg))
    {
        // Send response to application
        send(msg, "bmiOut");
    }
    else
    {
        cerr << "Unknown message type in BMI client\n";
    }
}

TCPSocket* BMITcpClient::getConnectedSocket(const FSHandle& handle)
{
    IPvXAddress* serverIp = PFSUtils::instance().getServerIP(handle);
    TCPSocket* sock = serverConnectionMap_.getSocket(serverIp->str());

    // If a connected socket does not exist, create it
    if (0 == sock)
    {
        sock = new TCPSocket();
        sock->setOutputGate(gate("tcpOut"));
        sock->setCallbackObject(this, NULL);
        sock->connect(serverIp->get4(), connectPort_);

        // Add open socket for use in later communication
        serverConnectionMap_.addSocket(serverIp->str(), sock);

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
    // Decapsulate the payload and call handleMessage with the payload
    cMessage* payload = msg->decapsulate();
    delete msg;
    handleMessage(payload);
}

void BMITcpClient::socketStatusArrived(int connId,
                                       void *yourPtr,
                                       TCPStatusInfo* status)
{
    cerr << "Socket status arrived: State: " << status->state()
         << " Name: " << status->stateName()
         << " MSS: " << status->snd_mss()
         << " Final Ack: " << status->fin_ack_rcvd() << endl;
    delete status;
}
/*
 * Local variables:
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 *
 * vim: ts=8 sts=4 sw=4 expandtab
 */
