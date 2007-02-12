#include <iostream>
#include "mpiio_proto_m.h"
#include "umd_io_trace.h"
#include <omnetpp.h>
using namespace std;

/**
 * Model of an application process.
 */
class App : public cSimpleModule
{
public:
    /** Constructor */
    App() : cSimpleModule(), trace_(0), rank_(-1) {};
    
protected:
    /** Implementation of initialize */
    virtual void initialize();

    /** Implementation of finish */
    virtual void finish();

    /** Implementation of handleMessage */
    virtual void handleMessage(cMessage* msg);

private:
    IOTrace* trace_;
    int rank_;
};

// OMNet Registriation Method
Define_Module(App);

static int rank_seed = 0;

/**
 * Construct an I/O trace using configuration supplied tracefile(s)
 */
void App::initialize()
{
    // Set the process rank
    rank_ = rank_seed++;
    
    // Construct a trace and begin sending events
    trace_ = new UMDIOTrace(8, "cholesky.trace");

    // Get the first event for this rank
    IOTraceRecord* rec = trace_->nextRecord();

    // Construct the appropriate MPI-IO event
    cMessage* mpiMsg = 0;//UMDIOTrace::convertToMPIEvent(rec);
    delete rec;

    // Send the message
    scheduleAt(5.0, mpiMsg);
}

/**
 * Cleanup trace and tally statistics
 */
void App::finish()
{
    delete trace_;
    trace_ = 0;

    // Reset the rank generator to 0
    rank_seed = 0;
}

/**
 * Handle MPI-IO Response messages
 */
void App::handleMessage(cMessage* msg)
{
    switch(msg->kind())
    {
        case MPI_FILE_OPEN_RESPONSE:
            cerr << "handleMessage not yet implemented for kind: "
                 << msg->kind() << endl;
            break;
        case MPI_FILE_CLOSE_RESPONSE:
            cerr << "handleMessage not yet implemented for kind: "
                 << msg->kind() << endl;
            break;
        case MPI_FILE_DELETE_RESPONSE:
            cerr << "handleMessage not yet implemented for kind: "
                 << msg->kind() << endl;
            break;
        case MPI_FILE_SET_SIZE_RESPONSE:
             cerr << "handleMessage not yet implemented for kind: "
                 << msg->kind() << endl;
           break;
        case MPI_FILE_PREALLOCATE_RESPONSE:
            cerr << "handleMessage not yet implemented for kind: "
                 << msg->kind() << endl;
            break;
        case MPI_FILE_GET_SIZE_RESPONSE:
            cerr << "handleMessage not yet implemented for kind: "
                 << msg->kind() << endl;
            break;
        case MPI_FILE_GET_INFO_RESPONSE:
            cerr << "handleMessage not yet implemented for kind: "
                 << msg->kind() << endl;
            break;
        case MPI_FILE_SET_INFO_RESPONSE:
            cerr << "handleMessage not yet implemented for kind: "
                 << msg->kind() << endl;
            break;
        case MPI_FILE_READ_AT_RESPONSE:
            cerr << "handleMessage not yet implemented for kind: "
                 << msg->kind() << endl;
            break;
        case MPI_FILE_READ_RESPONSE:
            cerr << "handleMessage not yet implemented for kind: "
                 << msg->kind() << endl;
            break;
        case MPI_FILE_WRITE_AT_RESPONSE:
            cerr << "handleMessage not yet implemented for kind: "
                 << msg->kind() << endl;
            break;
        case MPI_FILE_WRITE_RESPONSE:
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
