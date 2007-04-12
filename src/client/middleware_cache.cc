#include <iostream>
#include "mpiio_proto_m.h"
#include "umd_io_trace.h"
#include <omnetpp.h>
using namespace std;

/**
 * Model of a middleware file system data cache.
 */
class MiddlewareCache : public cSimpleModule
{
public:
    /** Constructor */
    MiddlewareCache() : cSimpleModule() {};
    
protected:
    /** Implementation of initialize */
    virtual void initialize() {};

    /** Implementation of finish */
    virtual void finish() {};

    /** Implementation of handleMessage */
    virtual void handleMessage(cMessage* msg);

};

// OMNet Registriation Method
Define_Module(MiddlewareCache);

/**
 * Handle MPI-IO Response messages
 */
void MiddlewareCache::handleMessage(cMessage* msg)
{
    switch(msg->kind())
    {
        case SPFS_MPI_FILE_OPEN_REQUEST:
            cerr << "handleMessage not yet implemented for kind: "
                 << msg->kind() << endl;
            break;
        case SPFS_MPI_FILE_CLOSE_REQUEST:
            cerr << "handleMessage not yet implemented for kind: "
                 << msg->kind() << endl;
            break;
        case SPFS_MPI_FILE_DELETE_REQUEST:
            cerr << "handleMessage not yet implemented for kind: "
                 << msg->kind() << endl;
            break;
        case SPFS_MPI_FILE_SET_SIZE_REQUEST:
             cerr << "handleMessage not yet implemented for kind: "
                 << msg->kind() << endl;
           break;
        case SPFS_MPI_FILE_PREALLOCATE_REQUEST:
            cerr << "handleMessage not yet implemented for kind: "
                 << msg->kind() << endl;
            break;
        case SPFS_MPI_FILE_GET_SIZE_REQUEST:
            cerr << "handleMessage not yet implemented for kind: "
                 << msg->kind() << endl;
            break;
        case SPFS_MPI_FILE_GET_INFO_REQUEST:
            cerr << "handleMessage not yet implemented for kind: "
                 << msg->kind() << endl;
            break;
        case SPFS_MPI_FILE_SET_INFO_REQUEST:
            cerr << "handleMessage not yet implemented for kind: "
                 << msg->kind() << endl;
            break;
        case SPFS_MPI_FILE_READ_AT_REQUEST:
            cerr << "handleMessage not yet implemented for kind: "
                 << msg->kind() << endl;
            break;
        case SPFS_MPI_FILE_READ_REQUEST:
            cerr << "handleMessage not yet implemented for kind: "
                 << msg->kind() << endl;
            break;
        case SPFS_MPI_FILE_WRITE_AT_REQUEST:
            cerr << "handleMessage not yet implemented for kind: "
                 << msg->kind() << endl;
            break;
        case SPFS_MPI_FILE_WRITE_REQUEST:
            cerr << "handleMessage not yet implemented for kind: "
                 << msg->kind() << endl;
            break;
        default:
            cerr << "handleMessage not yet implemented for kind: "
                 << msg->kind() << endl;
            break;
    }
}

/*
 * Local variables:
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 *
 * vim: ts=8 sts=4 sw=4 expandtab
 */
