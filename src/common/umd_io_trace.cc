//
// This file is part of Hecios
//
// Copyright (C) 2007 Brad Settlemyer
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
//
#include "umd_io_trace.h"
#include <cassert>
#include <iomanip>
#include <sstream>
#include "filename.h"
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
