
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
      fileNames_(0),
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
        fileNames_ = new string[numFiles_];
        for (int i = 0; i < numFiles_; i++)
        {
            int length;
            traceFile_ >> length;
            traceFile_ >> fileNames_[i];
            //cerr << "Filename: " << fileNames_[i] << endl;
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
    delete[] fileNames_;
}

/**
 * Construct the next record from the trace and return a pointer to it
 */
IOTraceRecord* UMDIOTrace::nextRecord()
{
    return 0;
}

cMessage* UMDIOTrace::nextRecordAsMessage()
{
    cMessage* msg = 0;
    
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
        msg = createMPIIOMessage(static_cast<OpType>(opType), fileId,
                                       offset, length);
        // Increment the current record
        curRecord_++;    
    }
    return msg;
}

cMessage* UMDIOTrace::createMPIIOMessage(OpType opType, int fileId,
                                         long offset, long length)
{
    cMessage* mpiMsg = 0;

    // Create the correct messages for each operation type
    switch(opType) {
        case UMDIOTrace::OPEN:
        {
            // If the file does not exist, create it
            Filename openFile(fileNames_[fileId]);
            if (!PFSUtils::instance().fileExists(openFile))
            {
                // Create trace files as needed
                PFSUtils::instance().createFile(openFile, 0, 1);
            }
            
            spfsMPIFileOpenRequest* open = new spfsMPIFileOpenRequest(
                0, SPFS_MPI_FILE_OPEN_REQUEST);
            open->setFileName(fileNames_[fileId].c_str());

            // Construct a file descriptor for use in simulaiton
            FSOpenFile* descriptor = new FSOpenFile();
            setDescriptor(fileId, descriptor);
            open->setFileDes(descriptor);
            mpiMsg = open;
            break;
        }
        case UMDIOTrace::CLOSE:
        {
            spfsMPIFileCloseRequest* close = new spfsMPIFileCloseRequest(
                0, SPFS_MPI_FILE_CLOSE_REQUEST);
            mpiMsg = close;
            break;
        }
        case UMDIOTrace::READ:
        {
            spfsMPIFileReadAtRequest* read = new spfsMPIFileReadAtRequest(
                0, SPFS_MPI_FILE_READ_AT_REQUEST);
            read->setCount(length);
            read->setOffset(offset);
            FSOpenFile* descriptor = getDescriptor(fileId);
            read->setFileDes(descriptor);
            mpiMsg = read;
            break;
        }
        case UMDIOTrace::WRITE:
        {
            spfsMPIFileWriteAtRequest* write = new spfsMPIFileWriteAtRequest(
                0, SPFS_MPI_FILE_WRITE_AT_REQUEST);
            write->setCount(length);
            write->setOffset(offset);
            FSOpenFile* descriptor = getDescriptor(fileId);
            assert(0 != descriptor);
            
            write->setFileDes(descriptor);
            mpiMsg = write;
            break;
        }
        case UMDIOTrace::SEEK:
        {
            //spfsMPIFileReadAtRequest* seek = new spfsMPIFileReadAtRequest(
            //    0, SPFS_MPI_FILE_READ_AT_REQUEST);
            //seek->setCount(0);
            //seek->setOffset(offset);
            // FIXME just adding a descript to avoid core dumps for now
            //FSOpenFile* descriptor = getDescriptor(fileId);
            //seek->setFiledes(descriptor);
            //mpiMsg = seek;
            break;
        }
        case UMDIOTrace::LISTIO_HEADER:
        default:
            cerr << "Ignored IO OpType for UMDIOTrace: " << opType << endl;
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
