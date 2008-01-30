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
#include "shtf_io_trace.h"
#include <cassert>
#include <iomanip>
#include <sstream>
#include "filename.h"
#include "pfs_utils.h"
using namespace std;

/**
 * Construct the file names from a single trace file
 */
SHTFIOTrace::SHTFIOTrace(const string& traceFileName)
    : IOTrace(1),
      traceFileName_(traceFileName),
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
        // The first line should contain the trace format
        string traceMeta;
        getline(traceFile_, traceMeta);
        
        // Read the header info
        traceFile_ >> numFiles_;
        traceFile_ >> numRecords_;

        // Read the file names
        for (int i = 0; i < numFiles_; i++)
        {
            string filename;
            size_t length;
            traceFile_ >> filename >> length;
            registerFile(filename, length);

            // Discard the trailing \n
            string tmp;
            getline(traceFile_, tmp);
        }
        curRecord_ = 0;
    }
    else
    {
        cerr << "Unable to open trace: " << traceFileName_ << endl;
    }
}

/** Cleanup open file and dynamic memory */
SHTFIOTrace::~SHTFIOTrace()
{
    traceFile_.close();
}

bool SHTFIOTrace::hasMoreRecords() const
{
    return (curRecord_ < numRecords_);
}

/**
 * Construct the next record from the trace and return a pointer to it
 */
IOTrace::Record* SHTFIOTrace::nextRecord()
{
    IOTrace::Record* rec = 0;
    if (curRecord_ < numRecords_)
    {
        string line;
        getline(traceFile_, line);
        
        // Create a new message and fill it out with the relevant data
        istringstream recordStream(line);
        rec = createIOTraceRecord(recordStream);

        // Increment the current record count
        curRecord_++;    
    }
    
    return rec;
}

IOTrace::Record* SHTFIOTrace::createIOTraceRecord(istream& recordStream)
{
    IOTrace::Record* rec = 0;

    // Peel off the start time and duration
    double startTime, duration;
    recordStream >> startTime >> duration;

    // Get the command name to build trace records with
    string token;
    recordStream >> token;

    if ("CLOSE" == token)
    {
        int descriptor, status;
        recordStream >> descriptor >> status;
        rec = createCloseRecord(descriptor, status, startTime, duration);
    }
    else if ("DELETE" == token)
    {
        string filename;
        recordStream >> filename;
        rec = createDeleteRecord(filename, startTime, duration);
    }
    else if ("MKDIR" == token)
    {
        string filename, perms;
        recordStream >> filename >> perms;
        rec = createMkdirRecord(filename, perms, startTime, duration);
    }
    else if ("OPEN" == token)
    {
        string filename, mode;
        int descriptor;
        recordStream >> filename >> mode >> descriptor;
        rec = createOpenRecord(filename, mode, descriptor, startTime, duration);
    }
    else if ("READ" == token)
    {
        int descriptor;
        size_t offset, extent;
        recordStream >> descriptor >> offset >> extent;
        rec = createReadRecord(descriptor, offset, extent,
                               startTime, duration);
    }
    else if ("UTIME" == token)
    {
        string filename, utime, tmp;
        recordStream >> filename;
        getline(recordStream, tmp, '\"');
        getline(recordStream, utime, '\"');
        rec = createUtimeRecord(filename, utime, startTime, duration);
    }
    else if ("WRITE" == token)
    {
        int descriptor;
        size_t offset, extent;
        recordStream >> descriptor >> offset >> extent;
        rec = createWriteRecord(descriptor, offset, extent,
                                startTime, duration);
    }

    return rec;
}

IOTrace::Record* SHTFIOTrace::createCloseRecord(int descriptor,
                                                int status,
                                                double startTime,
                                                double duration)
{
    IOTrace::Record* rec = new IOTrace::Record(IOTrace::CLOSE,
                                               startTime, duration);
    rec->fileId(descriptor);
    rec->filename(getFilename(descriptor));
    return rec;
}

IOTrace::Record* SHTFIOTrace::createDeleteRecord(const string& filename,
                                                 double startTime,
                                                 double duration)
{
    IOTrace::Record* rec = new IOTrace::Record(IOTrace::DELETE,
                                               startTime, duration);
    rec->filename(filename);
    return rec;
}

IOTrace::Record* SHTFIOTrace::createMkdirRecord(const string& filename,
                                                const string& perms,
                                                double startTime,
                                                double duration)
{
    IOTrace::Record* rec = new IOTrace::Record(IOTrace::MKDIR,
                                               startTime, duration);
    return rec;
}

IOTrace::Record* SHTFIOTrace::createOpenRecord(const string& filename,
                                               const string& mode,
                                               int descriptor,
                                               double startTime,
                                               double duration)
{
    // Update bookkeeping information
    addFilename(descriptor, filename);
    IOTrace::Record* rec = new IOTrace::Record(IOTrace::OPEN,
                                               startTime, duration);
    rec->fileId(descriptor);
    rec->filename(filename);
    return rec;
}

IOTrace::Record* SHTFIOTrace::createReadAtRecord(int descriptor,
                                                 size_t offset,
                                                 size_t extent,
                                                 double startTime,
                                                 double duration)
{
    IOTrace::Record* rec = new IOTrace::Record(IOTrace::READ_AT,
                                               startTime, duration);
    rec->fileId(descriptor);
    rec->filename(getFilename(descriptor));
    rec->offset(offset);
    rec->length(extent);
    return rec;
}

IOTrace::Record* SHTFIOTrace::createReadRecord(int descriptor,
                                               size_t offset,
                                               size_t extent,
                                               double startTime,
                                               double duration)
{
    IOTrace::Record* rec = new IOTrace::Record(IOTrace::READ,
                                               startTime, duration);
    rec->fileId(descriptor);
    rec->filename(getFilename(descriptor));
    rec->offset(offset);
    rec->length(extent);
    return rec;
}

IOTrace::Record* SHTFIOTrace::createStatRecord(double startTime,
                                               double duration)
{
    IOTrace::Record* rec = new IOTrace::Record(IOTrace::STAT,
                                               startTime, duration);
    return rec;
}

IOTrace::Record* SHTFIOTrace::createUtimeRecord(const string& filename,
                                                const string& time,
                                                double startTime,
                                                double duration)
{
    IOTrace::Record* rec = new IOTrace::Record(IOTrace::UTIME,
                                               startTime, duration);
    rec->filename(filename);
    return rec;
}

IOTrace::Record* SHTFIOTrace::createWriteAtRecord(int descriptor,
                                                  size_t offset,
                                                  size_t extent,
                                                  double startTime,
                                                  double duration)
{
    IOTrace::Record* rec = new IOTrace::Record(IOTrace::WRITE_AT,
                                               startTime, duration);
    rec->fileId(descriptor);
    rec->filename(getFilename(descriptor));
    rec->offset(offset);
    rec->length(extent);
    return rec;
}

IOTrace::Record* SHTFIOTrace::createWriteRecord(int descriptor,
                                                size_t offset,
                                                size_t extent,
                                                double startTime,
                                                double duration)
{
    IOTrace::Record* rec = new IOTrace::Record(IOTrace::WRITE,
                                               startTime, duration);
    rec->fileId(descriptor);
    rec->filename(getFilename(descriptor));
    rec->offset(offset);
    rec->length(extent);
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
