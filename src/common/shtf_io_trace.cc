//
// This file is part of Hecios
//
// Copyright (C) 2007,2008,2009 Brad Settlemyer
//
// This file is distributed WITHOUT ANY WARRANTY. See the file 'License.txt'
// for details on this and other legal matters.
//
#include "shtf_io_trace.h"
#include <cassert>
#include <iomanip>
#include <sstream>
#include "filename.h"
#include "spfs_exceptions.h"
using namespace std;

/**
 * Construct the file names from a single trace file
 */
SHTFIOTrace::SHTFIOTrace(const string& traceFileName)
    : IOTrace(1),
      traceFileName_(traceFileName),
      numFiles_(0),
      numRecords_(-1),
      offsetToTraceRecords_(0),
      nextRecord_(-1)
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

        // Read the file and directory names
        for (int i = 0; i < numFiles_; i++)
        {
            string filename;
            string type;
            traceFile_ >> filename >> type;

            if ("F" == type)
            {
                size_t size;
                traceFile_ >> size;
                registerFile(filename, size);
            }
            else
            {
                assert("D" == type);
                size_t numEntries;
                traceFile_ >> numEntries;
                registerDirectory(filename, numEntries);
            }

            // Discard the trailing \n
            string tmp;
            getline(traceFile_, tmp);
        }
        nextRecord_ = 1;
    }
    else
    {
        cerr << __FILE__ << ":" << __LINE__ << ":"
             <<"ERROR: Unable to open trace: " << traceFileName_ << endl;
        throw(NoSuchTraceFile(traceFileName));
    }
}

/** Cleanup open file and dynamic memory */
SHTFIOTrace::~SHTFIOTrace()
{
    traceFile_.close();
}

bool SHTFIOTrace::hasMoreRecords() const
{
    return (nextRecord_ <= numRecords_);
}

/**
 * Construct the next record from the trace and return a pointer to it
 */
IOTrace::Record* SHTFIOTrace::nextRecord()
{
    assert(true == hasMoreRecords());

    string line;
    getline(traceFile_, line);

    // Create a new message and fill it out with the relevant data
    istringstream recordStream(line);
    IOTrace::Record* rec = createIOTraceRecord(recordStream);
    if (0 == rec)
    {
        cerr << __FILE__ << ":" << __LINE__ << ":"
             << "Unable to construct record from: " << line << endl;
    }

    // Increment the next record ptr
    nextRecord_++;

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

    if ("ACCESS" == token)
    {
        string filename;
        recordStream >> filename;
        rec = createAccessRecord(filename, startTime, duration);
    }
    else if ("CLOSE" == token)
    {
        int descriptor, status;
        recordStream >> descriptor >> status;
        rec = createCloseRecord(descriptor, status, startTime, duration);
    }
    else if ("CPU_PHASE" == token)
    {
        rec = createCpuPhaseRecord(startTime, duration);
    }
    else if ("FCNTL" == token)
    {
        // TODO: Find a reasonable thing to do here
        static bool printWarning = true;
        if (printWarning)
        {
            cerr << __FILE__ << ":" << __LINE__ << ":"
                 << "WARNING: Converting FCNTL into CPU_PHASE\n";
            printWarning = false;
        }
        rec = createCpuPhaseRecord(startTime, duration);
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
    else if ("READDIR" == token)
    {
        int descriptor;
        size_t count;
        recordStream >> descriptor >> count;
        rec = createReadDirRecord(descriptor, count, startTime, duration);
    }
    else if ("RMDIR" == token)
    {
        string dirName;
        recordStream >> dirName;
        rec = createRmDirRecord(dirName, startTime, duration);
    }
    else if ("STAT" == token)
    {
        string filename;
        recordStream >> filename;
        rec = createStatRecord(filename, startTime, duration);
    }
    else if ("UNLINK" == token)
    {
        string filename;
        recordStream >> filename;
        rec = createUnlinkRecord(filename, startTime, duration);
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

IOTrace::Record* SHTFIOTrace::createAccessRecord(const string& filename,
                                                 double startTime,
                                                 double duration)
{
    IOTrace::Record* rec = new IOTrace::Record(IOTrace::ACCESS,
                                               startTime, duration);
    rec->filename(filename);
    return rec;
}

IOTrace::Record* SHTFIOTrace::createCpuPhaseRecord(double startTime,
                                                   double duration)
{
    IOTrace::Record* rec = new IOTrace::Record(IOTrace::CPU_PHASE,
                                               startTime, duration);
    return rec;
}

IOTrace::Record* SHTFIOTrace::createMkdirRecord(const string& dirName,
                                                const string& perms,
                                                double startTime,
                                                double duration)
{
    IOTrace::Record* rec = new IOTrace::Record(IOTrace::MKDIR,
                                               startTime, duration);
    rec->filename(dirName);
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

    // Set the open mode flags
    if (string::npos != mode.find("O_CREAT"))
    {
        rec->isCreate(true);
    }

    if (string::npos != mode.find("O_EXCL"))
    {
        rec->isExclusive(true);
    }

    if (string::npos != mode.find("O_RDONLY"))
    {
        rec->isReadOnly(true);
    }

    if (string::npos != mode.find("O_RDWR"))
    {
        rec->isReadWrite(true);
    }

    if (string::npos != mode.find("O_WRONLY"))
    {
        rec->isWriteOnly(true);
    }

    if (string::npos != mode.find("O_APPEND"))
    {
        rec->isAppend(true);
    }

    if (descriptor < 0)
    {
        rec->fileExists(false);
    }
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

IOTrace::Record* SHTFIOTrace::createReadDirRecord(int descriptor,
                                                size_t count,
                                                double startTime,
                                                double duration)
{
    IOTrace::Record* rec = new IOTrace::Record(IOTrace::READDIR,
                                               startTime, duration);
    rec->fileId(descriptor);
    rec->filename(getFilename(descriptor));
    rec->count(count);
    return rec;
}

IOTrace::Record* SHTFIOTrace::createRmDirRecord(const string& dirName,
                                                  double startTime,
                                                  double duration)
{
    IOTrace::Record* rec = new IOTrace::Record(IOTrace::RMDIR,
                                               startTime, duration);
    rec->filename(dirName);
    return rec;
}

IOTrace::Record* SHTFIOTrace::createStatRecord(const string& filename,
                                               double startTime,
                                               double duration)
{
    IOTrace::Record* rec = new IOTrace::Record(IOTrace::STAT,
                                               startTime, duration);
    rec->filename(filename);
    return rec;
}

IOTrace::Record* SHTFIOTrace::createUnlinkRecord(const string& filename,
                                                 double startTime,
                                                 double duration)
{
    IOTrace::Record* rec = new IOTrace::Record(IOTrace::UNLINK,
                                               startTime, duration);
    rec->filename(filename);
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
