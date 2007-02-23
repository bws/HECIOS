#include <iostream>
#include "TCPSocket.h"
#include "TCPSocketMap.h"
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
    int listenPort_;
    TCPSocketMap socketMap_;
    TCPSocket listenSocket_;
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
}

/**
 * Handle MPI-IO Response messages
 */
void BMITcpServer::handleMessage(cMessage* msg)
{
    if (listenSocket_.belongsToSocket(msg))
        listenSocket_.processMessage(msg);
    else
    {
        cerr << "BMITcpServer handle message has been invoked." << endl;
    }
    
    switch(msg->kind())
    {
        case FS_CREATE_REQUEST:
            cerr << "handleMessage not yet implemented for kind: "
                 << msg->kind() << endl;
            break;
        case FS_REMOVE_REQUEST:
            cerr << "handleMessage not yet implemented for kind: "
                 << msg->kind() << endl;
            break;
        case FS_READ_REQUEST:
            cerr << "handleMessage not yet implemented for kind: "
                 << msg->kind() << endl;
            break;
        case FS_WRITE_REQUEST:
             cerr << "handleMessage not yet implemented for kind: "
                 << msg->kind() << endl;
           break;
        case FS_GET_ATTR_REQUEST:
            cerr << "handleMessage not yet implemented for kind: "
                 << msg->kind() << endl;
            break;
        case FS_SET_ATTR_REQUEST:
            cerr << "handleMessage not yet implemented for kind: "
                 << msg->kind() << endl;
            break;
        case FS_LOOKUP_PATH_REQUEST:
            cerr << "handleMessage not yet implemented for kind: "
                 << msg->kind() << endl;
            break;
        case FS_CREATE_DIR_ENT_REQUEST:
            cerr << "handleMessage not yet implemented for kind: "
                 << msg->kind() << endl;
            break;
        case FS_REMOVE_DIR_ENT_REQUEST:
            cerr << "handleMessage not yet implemented for kind: "
                 << msg->kind() << endl;
            break;
        case FS_CHANGE_DIR_ENT_REQUEST:
            cerr << "handleMessage not yet implemented for kind: "
                 << msg->kind() << endl;
            break;
        case FS_TRUNCATE_REQUEST:
            cerr << "handleMessage not yet implemented for kind: "
                 << msg->kind() << endl;
            break;
        case FS_MAKE_DIR_REQUEST:
            cerr << "handleMessage not yet implemented for kind: "
                 << msg->kind() << endl;
            break;
        default:
            cerr << "handleMessage not yet implemented for kind: "
                 << msg->kind() << endl;
            break;
    }
}

void BMITcpServer::socketDataArrived(int, void *, cMessage *msg, bool)
{    
    ev << "Received TCP data, " << msg->byteLength() << " bytes"
       << " Kind: " << msg->kind() << endl;
    delete msg;
}


/*
 * Local variables:
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 *
 * vim: ts=8 sts=4 sw=4 expandtab
 */
