#include <cassert>
#include <iostream>
#include <map>
#include "TCPSocket.h"
#include "TCPSocketMap.h"
#include "network_proto_m.h"
#include "mpi_proto_m.h"
#include <omnetpp.h>
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
    virtual void initialize();

    /** Implementation of finish */
    virtual void finish() {};

    /** Implementation of cSimpleModule::handleMessage */
    virtual void handleMessage(cMessage* msg);

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

    /** Mapping from a messages uniqueId to the amount of data received */
    std::map<int,TCPSocket*> requestToSocketMap_;

    /** Gate id for appOut */
    int appOutGateId_;
};

// OMNet Registriation Method
Define_Module(MPITcpServer);

void MPITcpServer::initialize()
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
    appOutGateId_ = gate("appOut")->id();
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
    else if (0 != dynamic_cast<spfsMPISendResponse*>(msg))
    {
        // Retrieve the socket
        spfsMPIRequest* origReq =
            static_cast<spfsMPIRequest*>(msg->contextPointer());
        map<int,TCPSocket*>::iterator pos =
            requestToSocketMap_.find(origReq->getSocketId());
        assert(requestToSocketMap_.end() != pos);
        
        TCPSocket* responseSocket = pos->second;
        assert(0 != responseSocket);

        // Remove the entry for this socket
        requestToSocketMap_.erase(origReq->getSocketId());

        // Encapsulate the file system response and send to the client
        spfsNetworkServerSendMessage* pkt = new spfsNetworkServerSendMessage();
        pkt->encapsulate(msg);
        pkt->setByteLength(4);
        pkt->setUniqueId(ev.getUniqueNumber());
        responseSocket->send(pkt);
     }
    else if (0 != dynamic_cast<spfsMPISendRequest*>(msg))
    {
        send(msg, appOutGateId_);
    }
    else
    {
        cerr << "MPI TCP Server does not support message type "
             << " name: " << msg->name()
             << " kind: " << msg->kind()
             << " info: " << msg->info() << endl;
    }
}

//
// TCP settings should be set to tcp.sendQueueClass="TCPMsgBasedSendQueue"
// and tcp.receiveQueueClass="TCPMsgBasedRcvQueue" ensuring that only
// whole messages are received rather than message fragments
//
void MPITcpServer::socketDataArrived(int, void *, cMessage *msg, bool)
{    
    // Find the socket for this message
    TCPSocket* responseSocket = socketMap_.findSocketFor(msg);
    
    // Decapsulate the payload and call handleMessage with the payload
    cMessage* payload = msg->decapsulate();

    // Store the response socket for use during response
    static int nextSocketId = 0;
    spfsMPIRequest* request = dynamic_cast<spfsMPIRequest*>(payload);
    request->setSocketId(nextSocketId++);
    requestToSocketMap_[request->getSocketId()] = responseSocket;
    
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