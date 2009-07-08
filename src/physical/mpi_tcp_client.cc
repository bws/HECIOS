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

    /** Mapping from a remote rank to the sending socket */
    std::map<int,TCPSocket*> rankToSocketMap_;

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
    TCPSocket* socket = 0;
    map<int,TCPSocket*>::const_iterator iter = rankToSocketMap_.find(rank);

    // If a connected socket does not exist, create it
    if (iter == rankToSocketMap_.end())
    {
        cerr << __FILE__ << ":" << __LINE__ << ":"
             << "creating socket to communicate to rank" << rank << endl;
        // Construct the socket
        socket = new TCPSocket();
        socket->setOutputGate(gate("tcpOut"));
        socket->setCallbackObject(this, NULL);

        // Set the connection destination
        PFSUtils::ConnectionDescriptor cd =
            PFSUtils::instance().getRankConnectionDescriptor(rank);
        IPvXAddress* ip = cd.first;
        size_t connectPort = cd.second;
        socket->connect(ip->get4(), connectPort);

        // Add open socket for use in later communication
        rankToSocketMap_[rank] = socket;

        // Add open socket to TCPSocketMap for handling later TCP messages
        socketMap_.addSocket(socket);
    }
    else
    {
        cerr << __FILE__ << ":" << __LINE__ << ":"
             << "Using existing socket to communicate with rank: " << rank << endl;
        socket = iter->second;
    }
    return socket;
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
