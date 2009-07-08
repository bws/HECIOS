//
// This file is part of Hecios
//
// Copyright (C) 2007,2008,2009 Brad Settlemyer
//
// This file is distributed WITHOUT ANY WARRANTY. See the file 'License.txt'
// for details on this and other legal matters.
//
#include "umd_io_trace.h"
#include <cassert>
#include <iomanip>
#include <sstream>
#include "filename.h"
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

            // Add the file to the filesystem with an arbitrary size
            addFilename(i, filename);
            registerFile(filename, 100000);
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
IOTrace::Record* UMDIOTrace::nextRecord()
{
    IOTrace::Record* rec = 0;
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

        // Read the tracefile record into a string and then parse so that
        // we can keep a copy of what was parsed.  For time and space
        // efficiency it may be necessary to simplify this later
        string sourceLine;
        if ('\n' == traceFile_.peek())
        {
            getline(traceFile_, sourceLine);
        }
        getline(traceFile_, sourceLine);
        istringstream source(sourceLine);

        // Read the tracefile record in
        source >> opType;
        source >> numRecords;
        source >> pid >> fileId >> wallClock >> processClock;
        source >> offset >> length;

        // Create a new message and fill it out with the relevant data
        rec = createIOTraceRecord(static_cast<OpType>(opType), fileId,
                                  offset, length);

        // Add the trace source to the record
        if (0 != rec)
            rec->setSource(sourceLine);

        // Increment the current record count
        curRecord_++;
    }

    return rec;
}

IOTrace::Record* UMDIOTrace::createIOTraceRecord(OpType opType, int fileId,
                                                 long offset, long length)
{
    IOTrace::Record* rec = 0;

    // Create the correct messages for each operation type
    switch(opType) {
        case UMDIOTrace::OPEN:
        {
            rec = new IOTrace::Record(IOTrace::OPEN, fileId, 0, 0);
            break;
        }
        case UMDIOTrace::CLOSE:
        {
            rec = new IOTrace::Record(IOTrace::CLOSE, fileId, 0, 0);
            break;
        }
        case UMDIOTrace::SEEK:
        {
            rec = new IOTrace::Record(IOTrace::SEEK, fileId, offset, length);
            break;
        }
        case UMDIOTrace::READ:
        {
            rec = new IOTrace::Record(IOTrace::READ_AT, fileId, offset, length);
            break;
        }
        case UMDIOTrace::WRITE:
        {
            rec = new IOTrace::Record(
                IOTrace::WRITE_AT, fileId, offset, length);
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
 *  indent-tabs-mode: nil
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 *
 * vim: ts=8 sts=4 sw=4 expandtab
 */
