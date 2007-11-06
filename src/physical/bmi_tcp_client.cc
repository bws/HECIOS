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

    /** Number of bytes required as overhead to use the BMI protocol */
    static const unsigned int TCP_OVERHEAD_BYTES = 4;
    
    /** Constructor */
    BMITcpClient() : BMIEndpoint() {};
    
    /** @return a BMIExpected message encapsulating msg */
    virtual spfsBMIExpectedMessage* createExpectedMessage(cMessage* msg);
    
    /** @return a BMIUnexpected message encapsulating msg */
    virtual spfsBMIUnexpectedMessage* createUnexpectedMessage(
        spfsRequest* request);
    
    /** @return a PullDataResponse for the request */
    virtual spfsBMIPullDataResponse* createPullDataResponse(
        spfsBMIPullDataRequest* pullRequest);
        
    /** @return a PushDataResponse for the request */
    virtual spfsBMIPushDataResponse* createPushDataResponse(
        spfsBMIPushDataRequest* pushRequest);
        
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
    
    /**
     * Indicates the receipt of a data flow pull request
     *
     * Send a simple reply over the network and trigger flow completion
     * if warranted
     */
    virtual void pullDataRequestReceived(spfsBMIPullDataRequest* pullRequest);
    
    /** 
     * Indicates the receipt of a data flow pull request
     *
     * Send a simple reply over the network and trigger flow completion
     * if warranted
     */
    virtual void pushDataRequestReceived(spfsBMIPushDataRequest* pushRequest);

    /** Extract the payload from a completed socket message */
    void socketDataArrived(int, void *, cMessage *msg, bool);

    /** Handle the arrival of status messages to note when a msg is recvd */
    void socketStatusArrived(int connId, void *yourPtr, TCPStatusInfo *status);
    
private:

    /** @return a socket with an open connection to the server */
    TCPSocket* getConnectedSocket(const FSHandle& handle);

    /**
     * Update the amount of flow data transferred for the flowId
     *
     * @return the total data transferred as part of the flow
     */
    FSSize updateFlowProgress(int flowId, FSSize transferSize);
    
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

    /** Map containing the progress of flows */
    std::map<int, FSSize> flowProgressById_;
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
    pkt->setByteLength(BMI_OVERHEAD_BYTES + TCP_OVERHEAD_BYTES);
    pkt->encapsulate(request);
    return pkt;
}

spfsBMIExpectedMessage* BMITcpClient::createExpectedMessage(cMessage* msg)
{
    assert(0 != msg);
    cerr << __FILE__ << ":" << __LINE__ << ":"
         << "TCPClient is unable to create expected messages" << endl;
    return 0;
}

spfsBMIPullDataResponse* BMITcpClient::createPullDataResponse(
    spfsBMIPullDataRequest* pullRequest)
{
    assert(0 != pullRequest);

    // Extract the relevant data from the request
    //int connectionId = pullRequest->getConnectionId();
    int flowId = pullRequest->getFlowId();
    //FSSize flowSize = pullRequest->getFlowSize();
    FSSize dataSize = pullRequest->getRequestSize();
    FSHandle handle = pullRequest->getHandle();

    // Create the response
    spfsBMIPullDataResponse* pullResp = new spfsBMIPullDataResponse();
    pullResp->setContextPointer(pullRequest);
    pullResp->setConnectionId(handle);
    pullResp->setFlowId(flowId);
    pullResp->setDataSize(dataSize);
    pullResp->setByteLength(dataSize);
    return pullResp;
}

spfsBMIPushDataResponse* BMITcpClient::createPushDataResponse(
    spfsBMIPushDataRequest* pushRequest)
{
    assert(0 != pushRequest);

    // Extract the relevant data from the request
    //int connectionId = pushRequest->getConnectionId();
    int flowId = pushRequest->getFlowId();
    //FSSize flowSize = pushRequest->getFlowSize();
    FSSize dataSize = pushRequest->getDataSize();
    FSHandle handle = pushRequest->getHandle();

    spfsBMIPushDataResponse* pushResp = new spfsBMIPushDataResponse();
    pushResp->setContextPointer(pushRequest);
    pushResp->setConnectionId(handle);
    pushResp->setFlowId(flowId);
    pushResp->setReceivedSize(dataSize);
    pushResp->setByteLength(sizeof(FSSize));
    return pushResp;
}

void BMITcpClient::sendOverNetwork(spfsBMIExpectedMessage* msg)
{
    assert(0 != msg);
    msg->addByteLength(BMI_OVERHEAD_BYTES + TCP_OVERHEAD_BYTES);
    
    // Retrieve the socket for this handle
    TCPSocket* sock = getConnectedSocket(msg->getConnectionId());
    sock->send(msg);
}

void BMITcpClient::sendOverNetwork(spfsBMIUnexpectedMessage* msg)
{
    assert(0 != msg);
    msg->addByteLength(BMI_OVERHEAD_BYTES + TCP_OVERHEAD_BYTES);
    
    // Retrieve the socket for this handle
    TCPSocket* sock = getConnectedSocket(msg->getHandle());
    sock->send(msg);
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

void BMITcpClient::pullDataRequestReceived(spfsBMIPullDataRequest* pullRequest)
{
    // Send the response to the server
    BMIEndpoint::pullDataRequestReceived(pullRequest);

    // Update flow progress
    int flowId = pullRequest->getFlowId();
    FSSize dataSize = pullRequest->getRequestSize();
    FSSize flowProgress = updateFlowProgress(flowId, dataSize);
    assert(flowProgress <= pullRequest->getFlowSize());

    // Check for flow completion
    if (pullRequest->getFlowSize() == flowProgress)
    {
        spfsDataFlowFinish* finishMsg =
            new spfsDataFlowFinish(0, SPFS_DATA_FLOW_FINISH);
        cMessage* flowStart =
            static_cast<cMessage*>(pullRequest->contextPointer());
        finishMsg->setContextPointer(flowStart->contextPointer());
        finishMsg->setFlowId(flowId);
        finishMsg->setFlowSize(flowProgress);
        send(finishMsg, "appOut");
    }
}

void BMITcpClient::pushDataRequestReceived(spfsBMIPushDataRequest* pushRequest)
{
    // Send the response to the server
    BMIEndpoint::pushDataRequestReceived(pushRequest);

    // Update flow progress
    int flowId = pushRequest->getFlowId();
    FSSize dataSize = pushRequest->getDataSize();
    FSSize flowProgress = updateFlowProgress(flowId, dataSize);
    assert(flowProgress <= pushRequest->getFlowSize());

    // Check for flow completion
    if (pushRequest->getFlowSize() == flowProgress)
    {
        spfsDataFlowFinish* finishMsg =
            new spfsDataFlowFinish(0, SPFS_DATA_FLOW_FINISH);
        cMessage* flowStart =
            static_cast<cMessage*>(pushRequest->contextPointer());
        finishMsg->setContextPointer(flowStart->contextPointer());
        finishMsg->setFlowId(flowId);
        send(finishMsg, "appOut");
    }    
}

FSSize BMITcpClient::updateFlowProgress(int flowId, FSSize transferSize)
{
    FSSize totalProgress = 0;
    map<int, FSSize>::const_iterator pos = flowProgressById_.find(flowId);
    if (flowProgressById_.end() == pos)
    {
        totalProgress = (flowProgressById_[flowId] = transferSize);
    }
    else
    {
        totalProgress = (flowProgressById_[flowId] += transferSize);
    }
    
    return totalProgress;
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
 *  indent-tabs-mode: nil
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 *
 * vim: ts=8 sts=4 sw=4 expandtab
 */
