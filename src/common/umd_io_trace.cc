#include <iomanip>
#include "mpiio_proto_m.h"
#include "umd_io_trace.h"
using namespace std;

/**
 * Construct the file names from a single trace file
 */
UMDIOTrace::UMDIOTrace(int numProcs, string traceFileName)
    : IOTrace(numProcs),
      traceFileName_(traceFileName),
      numProcs_(0),
      numFiles_(0),
      numRecords_(0),
      curRecord_(-1)
{
    // Open the trace file
    traceFile_.open(traceFileName_.c_str());
    
    // Determine if the trace file opened successfully
    if (!traceFile_)
    {
        cerr << "Unable to open file: " << traceFileName_ << endl;
    }
    else
    {
        // Read the header info
        traceFile_ >> numProcs_;
        traceFile_ >> numFiles_;
        traceFile_ >> numRecords_;

        // Read the file names
        fileNames_ = new string[numFiles_];
        for (int i = 0; i < numFiles_; i++)
        {
            traceFile_ >> fileNames_[i];
        }
        curRecord_ = 0;
    }
}

/** Cleanup open file and dynamic memory */
UMDIOTrace::~UMDIOTrace()
{
    traceFile_.close();
    delete[] fileNames_;
}

/**
 * Construct the next record from the trace and return a pointer to it
 */
IOTraceRecord* UMDIOTrace::nextRecord() const
{
    return 0;
}

cMessage* UMDIOTrace::nextRecordAsMessage() const
{
    int opType;
    int pid;
    int fileId;
    double wallClock;
    double processClock;
    long offset;
    long length;

    // Read the tracefile record in
    traceFile_ >> opType;
    traceFile_ >> pid >> fileId >> wallClock >> processClock;
    traceFile_ >> offset >> length;

    // Create a new message and fill it out with the relevant data
    cMessage* msg = createMPIIOMessage(static_cast<OpType>(opType), fileId,
                                       offset, length);
    return msg;
}

cMessage* UMDIOTrace::createMPIIOMessage(OpType opType, int fileId,
                                         long offset, long length) const
{
    cMessage* mpiMsg = 0;

    // Create the correct messages for each operation type
    switch(opType) {
        case UMDIOTrace::OPEN:
        {
            mpiFileOpenRequest* open = new mpiFileOpenRequest(
                0, MPI_FILE_OPEN_REQUEST);
            open->setFileName(fileNames_[fileId].c_str());
            mpiMsg = open;
            break;
        }
        case UMDIOTrace::CLOSE:
        {
            mpiFileCloseRequest* close = new mpiFileCloseRequest(
                0, MPI_FILE_CLOSE_REQUEST);
            mpiMsg = close;
            break;
        }
        case UMDIOTrace::READ:
        {
            mpiFileReadRequest* read = new mpiFileReadRequest(
                0, MPI_FILE_READ_REQUEST);
            mpiMsg = read;
            break;
        }
        case UMDIOTrace::WRITE:
        {
            mpiFileWriteRequest* write = new mpiFileWriteRequest(
                0, MPI_FILE_WRITE_REQUEST);
            mpiMsg = write;
            break;
        }
        default:
            cerr << "Unrecognized IO OpType fo UMDIOTrace: " << opType << endl;
            break;
    }
    return mpiMsg;
}

/*
 * Local variables:
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 *
 * vim: ts=8 sts=4 sw=4 expandtab
 */
