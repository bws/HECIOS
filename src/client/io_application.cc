#include <iostream>
#include <sstream>
#include <string>
#include "mpiio_proto_m.h"
#include "umd_io_trace.h"
#include <omnetpp.h>
using namespace std;

/**
 * Model of an application process.
 */
class IOApplication : public cSimpleModule
{
public:
    /** Constructor */
    IOApplication() : cSimpleModule(), trace_(0), rank_(-1) {};
    
protected:
    /** Implementation of initialize */
    virtual void initialize();

    /** Implementation of finish */
    virtual void finish();

    /** Implementation of handleMessage */
    virtual void handleMessage(cMessage* msg);

private:

    void getNextMessage();
    
    IOTrace* trace_;
    int rank_;
};

// OMNet Registriation Method
Define_Module(IOApplication);

static int rank_seed = 0;

/**
 * Construct an I/O trace using configuration supplied tracefile(s)
 */
void IOApplication::initialize()
{
    // Set the process rank
    rank_ = rank_seed++;

    // Get the trace file name and perform the rank substitution
    string traceName = par("traceFile").stringValue();
    int replaceIdx = traceName.find("%r");
    if (string::npos != replaceIdx)
    {
        long numTraceProcs = par("numTraceProcs").longValue();
        long fileRank = rank_ % numTraceProcs;
        stringstream rankStr;
        rankStr << fileRank;
        traceName.replace(replaceIdx, 2, rankStr.str());
    }
    
    // Construct a trace and begin sending events
    long size = par("numTraceProcs").longValue();
    trace_ = new UMDIOTrace(size, traceName);

    // Get the first event for this rank
    cMessage* msg = trace_->nextRecordAsMessage();

    // Send the message
    scheduleAt(5.0, msg);
}

/**
 * Cleanup trace and tally statistics
 */
void IOApplication::finish()
{
    delete trace_;
    trace_ = 0;

    // Reset the rank generator to 0
    rank_seed = 0;
}

/**
 * Handle MPI-IO Response messages
 */
void IOApplication::handleMessage(cMessage* msg)
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

void IOApplication::getNextMessage()
{
    cMessage* msg = 0;
    
    do {
        msg = trace_->nextRecordAsMessage();
    } while (0 != msg && trace_->hasMoreRecords());

    if (0 != msg)
    {
        send(msg, "out");
    }
    else
    {
        cout << "IOApplication: No more messages to post." << endl;
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
