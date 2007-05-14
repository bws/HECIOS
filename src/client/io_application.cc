#include <iostream>
#include <sstream>
#include <string>
#include "client_fs_state.h"
#include "mpiio_proto_m.h"
#include "pfs_utils.h"
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

    /** Get the next message to send */
    virtual cMessage* getNextMessage();
    
private:

    IOTrace* trace_;
    int rank_;
    ClientFSState clientState_;

    int inGate_;
    int outGate_;
};

// OMNet Registriation Method
Define_Module(IOApplication);

static int rank_seed = 0;

/**
 * Construct an I/O trace using configuration supplied tracefile(s)
 */
void IOApplication::initialize()
{
    // Cache the gate descriptors
    inGate_ = findGate("in");
    outGate_ = findGate("out");
    
    // Set the process rank
    rank_ = rank_seed++;

    // Get the trace file name and perform the rank substitution
    string traceName = par("traceFile").stringValue();
    string::size_type replaceIdx = traceName.find("%r");
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
    if (msg->isSelfMessage())
    {
        // On a self message, forward the message to the "out" gate
        send(msg, outGate_);
    }
    else
    {
        switch(msg->kind())
        {
            case SPFS_MPI_FILE_OPEN_RESPONSE:
            case SPFS_MPI_FILE_CLOSE_RESPONSE:
            case SPFS_MPI_FILE_DELETE_RESPONSE:
            case SPFS_MPI_FILE_SET_SIZE_RESPONSE:
            case SPFS_MPI_FILE_PREALLOCATE_RESPONSE:
            case SPFS_MPI_FILE_GET_SIZE_RESPONSE:
            case SPFS_MPI_FILE_GET_INFO_RESPONSE:
            case SPFS_MPI_FILE_SET_INFO_RESPONSE:
            case SPFS_MPI_FILE_READ_AT_RESPONSE:
            case SPFS_MPI_FILE_READ_RESPONSE:
            case SPFS_MPI_FILE_WRITE_AT_RESPONSE:
            case SPFS_MPI_FILE_WRITE_RESPONSE:
            {
                // Send the next message
                cMessage* nextMsg = getNextMessage();
                if (0 != nextMsg)
                {
                    send(nextMsg, outGate_);
                }
                else
                {
                    cerr << "IOApplication: No more messages to post."
                         << endl;
                }
                break;
            }
            default:
                cerr << "IOApplication::handleMessage not yet implemented "
                     << "for kind: "<< msg->kind() << endl;
                break;
        }

        // Delete the message
        delete msg;
        
    }
}

cMessage* IOApplication::getNextMessage()
{
    cMessage* msg = 0;
    
    do {
        msg = trace_->nextRecordAsMessage();
    } while (0 == msg && trace_->hasMoreRecords());

    // Create the requested file if neccesary and it does not exist
    spfsMPIFileOpenRequest* open = 0;
    if ((open = dynamic_cast<spfsMPIFileOpenRequest*>(msg)) &&
        (open->getMode() == MPI_MODE_CREATE) &&
        (!PFSUtils::instance().fileExists(open->getFileName())) )
    {
        PFSUtils::instance().createFile(open->getFileName(), 1, 1);
    }
    
    return msg;
}
/*
 * Local variables:
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 *
 * vim: ts=8 sts=4 sw=4 expandtab
 */
