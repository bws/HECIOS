#include <cassert>
#include <iostream>
#include <map>
#include "TCPSocket.h"
#include "TCPSocketMap.h"
#include "bmi_proto_m.h"
#include "pvfs_proto_m.h"
#include "umd_io_trace.h"
#include <omnetpp.h>
using namespace std;

/**
 * Model of a file system client library.
 */
class BMITcpServer : public cSimpleModule, public TCPSocket::CallbackInterface
{
public:
    /** Constructor */
    BMITcpServer() : cSimpleModule() {};
    
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
    std::map<void*,TCPSocket*> contextToSocketMap;

    /** Gate id for bmiIn */
    int bmiInGateId_;

    /** Gate id for tcpIn */
    int tcpInGateId_;
};

// OMNet Registriation Method
Define_Module(BMITcpServer);

void BMITcpServer::initialize()
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
    bmiInGateId_ = gate("bmiIn")->id();
    tcpInGateId_ = gate("tcpIn")->id();

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
    else if (0 != dynamic_cast<spfsResponse*>(msg))
    {
        // Retrieve the socket
        spfsResponse* resp = dynamic_cast<spfsResponse*>(msg);
        map<void*,TCPSocket*>::iterator pos =
            contextToSocketMap.find(resp->contextPointer());
        assert(contextToSocketMap.end() != pos);
        
        TCPSocket* responseSocket = pos->second;
        assert(0 != responseSocket);

        // Remove the entry for this socket
        contextToSocketMap.erase(resp->contextPointer());

        // Encapsulate the file system response and send to the client
        spfsBMIServerSendMessage* pkt = new spfsBMIServerSendMessage();
        pkt->encapsulate(resp);
        pkt->setByteLength(256);
        pkt->setUniqueId(ev.getUniqueNumber());
        responseSocket->send(pkt);

        // A mostly ineffective hack to disable excessive INET output
        ev.disable_tracing = true;        
     }
    else if (0 != dynamic_cast<spfsRequest*>(msg))
    {
        cerr << "BMI Forwarding request" << endl;
        send(msg, "bmiOut");

        // A mostly ineffective hack to disable excessive INET output
        ev.disable_tracing = false;
    }
    else
    {
        cerr << "BMI Server does not support message type "
             << " name: " << msg->name()
             << " kind: " << msg->kind()
             << " info: " << msg->info() << endl;
    }
}

void BMITcpServer::socketDataArrived(int, void *, cMessage *msg, bool)
{    
    // TCP settings should be set to tcp.sendQueueClass="TCPMsgBasedSendQueue"
    // and tcp.receiveQueueClass="TCPMsgBasedRcvQueue" ensuring that only
    // whole messages are received rather than message fragments

    // Find the socket for this message
    TCPSocket* responseSocket = socketMap_.findSocketFor(msg);
    
    // Decapsulate the payload and call handleMessage with the payload
    cMessage* payload = msg->decapsulate();

    // Store the response socket for use during response
    spfsRequest* request = dynamic_cast<spfsRequest*>(payload);
    contextToSocketMap[request->contextPointer()] = responseSocket;
    
    //if (request)
    //    request->setSocket(responseSocket);
    //else
    //    cerr << "NON SPFS Request sent to BMI server" << endl;
    
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
