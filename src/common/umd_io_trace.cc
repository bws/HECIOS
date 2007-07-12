
#include "umd_io_trace.h"
#include <cassert>
#include <iomanip>
#include "filename.h"
#include "mpiio_proto_m.h"
#include "pfs_utils.h"
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
      offsetToTraceRecords_(0),
      curRecord_(-1)
{
    // Open the trace file
    traceFile_.open(traceFileName_.c_str(), ios::in|ios::binary);
    
    // Determine if the trace file opened successfully
    if (traceFile_)
    {
        string temp;
        
        // Read the header info
        traceFile_ >> numProcs_;
        traceFile_ >> numFiles_;
        traceFile_ >> numRecords_;

        // Read the file names
        for (int i = 0; i < numFiles_; i++)
        {
            int length;
            traceFile_ >> length;

            string filename;
            traceFile_ >> filename;
            addFilename(i, filename);
        }
        curRecord_ = 0;
    }
    else
    {
        cerr << "Unable to open trace: " << traceFileName_ << endl;
    }
}

/** Cleanup open file and dynamic memory */
UMDIOTrace::~UMDIOTrace()
{
    traceFile_.close();
}

/**
 * Construct the next record from the trace and return a pointer to it
 */
IOTraceRecord* UMDIOTrace::nextRecord()
{
    IOTraceRecord* rec = 0;
    if (curRecord_ < numRecords_)
    {
        int opType;
        int numRecords;  // This field seems irrelevant
        int pid;
        int fileId;
        double wallClock;
        double processClock;
        long offset;
        long length;

        // Read the tracefile record in
        traceFile_ >> opType;
        traceFile_ >> numRecords;
        traceFile_ >> pid >> fileId >> wallClock >> processClock;
        traceFile_ >> offset >> length;

        // Create a new message and fill it out with the relevant data
        rec = createIOTraceRecord(static_cast<OpType>(opType), fileId,
                                  offset, length);
        // Increment the current record
        curRecord_++;    
    }
    
    return rec;
}

IOTraceRecord* UMDIOTrace::createIOTraceRecord(OpType opType, int fileId,
                                              long offset, long length)
{
    IOTraceRecord* rec = 0;

    // Create the correct messages for each operation type
    switch(opType) {
        case UMDIOTrace::OPEN:
        {
            rec = new IOTraceRecord(IOTrace::OPEN, fileId, 0, 0);
            break;
        }
        case UMDIOTrace::CLOSE:
        {
            rec = new IOTraceRecord(IOTrace::CLOSE, fileId, 0, 0);
            break;
        }
        case UMDIOTrace::SEEK:
        {
            rec = new IOTraceRecord(IOTrace::SEEK, fileId, offset, length);
            break;
        }
        case UMDIOTrace::READ:
        {
            rec = new IOTraceRecord(IOTrace::READ_AT, fileId, offset, length);
            break;
        }
        case UMDIOTrace::WRITE:
        {
            rec = new IOTraceRecord(IOTrace::WRITE_AT, fileId, offset, length);
            break;
        }
        case UMDIOTrace::LISTIO_HEADER:
        default:
            cerr << "Ignored IO OpType for UMDIOTrace: " << opType << endl;
            break;
    }
    return rec;
}

/*
 * Local variables:
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 *
 * vim: ts=8 sts=4 sw=4 expandtab
 */
