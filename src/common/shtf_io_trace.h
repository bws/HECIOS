#ifndef SHTF_IO_TRACE_H
#define SHTF_IO_TRACE_H
//
// This file is part of Hecios
//
// Copyright (C) 2007,2008,2009 Brad Settlemyer
//
// This file is distributed WITHOUT ANY WARRANTY. See the file 'License.txt'
// for details on this and other legal matters.
//
#include <fstream>
#include <istream>
#include <string>
#include "io_trace.h"

/**
 * An I/O trace in the "Serial HECIOS Trace Format."
 *
 */
class SHTFIOTrace: public IOTrace
{
public:

    /** Constructor */
    SHTFIOTrace(const std::string& traceFileName);

    /** Destructor */
    virtual ~SHTFIOTrace();

    /** @return true if the trace file is valid */
    bool isValid() const {return traceFile_;};

    /** @return true if more records remain */
    bool hasMoreRecords() const;

    /** @return the number of records in the trace */
    std::size_t getNumRecords() const { return numRecords_; };

    /** @return the next IOTraceRecord */
    virtual IOTrace::Record* nextRecord();

private:
    /** @return the IOTraceRecord contained in the recordStream*/
    IOTrace::Record* createIOTraceRecord(std::istream& recordStream);

    /** @return an ACCESS IOTraceRecord */
    IOTrace::Record* createAccessRecord(const std::string& filename,
                                        double startTime,
                                        double duration);

    /** @return a CLOSE IOTraceRecord */
    IOTrace::Record* createCloseRecord(int descriptor,
                                       int status,
                                       double startTime,
                                       double duration);

    /** @return a CPU_PHASE IOTraceRecord */
    IOTrace::Record* createCpuPhaseRecord(double startTime,
                                          double duration);

    /** @return a DELETE IOTraceRecord */
    IOTrace::Record* createDeleteRecord(const std::string& filename,
                                        double startTime,
                                        double duration);

    /** @return a MKDIR IOTraceRecord */
    IOTrace::Record* createMkdirRecord(const std::string& filename,
                                       const std::string& perms,
                                       double startTime,
                                       double duration);

    /** @return a OPEN IOTraceRecord */
    IOTrace::Record* createOpenRecord(const std::string& filename,
                                      const std::string& mode,
                                      int descriptor,
                                      double startTime,
                                      double duration);

    /** @return a READ_AT IOTraceRecord */
    IOTrace::Record* createReadAtRecord(int descriptor,
                                        size_t offset,
                                        size_t extent,
                                        double startTime,
                                        double duration);

    /** @return a READ IOTraceRecord */
    IOTrace::Record* createReadRecord(int descriptor,
                                      size_t offset,
                                      size_t extent,
                                      double startTime,
                                      double duration);

    /** @return an READDIR IOTraceRecord */
    IOTrace::Record* createReadDirRecord(int descriptor,
                                         size_t count,
                                         double startTime,
                                         double duration);

    /** @return an RMDIR IOTraceRecord */
    IOTrace::Record* createRmDirRecord(const std::string& dirName,
                                       double startTime,
                                       double duration);

    /** @return a STAT IOTraceRecord */
    IOTrace::Record* createStatRecord(const std::string& filename,
                                      double startTime,
                                      double duration);

    /** @return a UNLINK IOTraceRecord */
    IOTrace::Record* createUnlinkRecord(const std::string& filename,
                                        double startTime,
                                        double duration);

    /** @return a UTIME IOTraceRecord */
    IOTrace::Record* createUtimeRecord(const std::string& filename,
                                       const std::string& time,
                                       double startTime,
                                       double duration);

    /** @return a WRITE_AT IOTraceRecord */
    IOTrace::Record* createWriteAtRecord(int descriptor,
                                         size_t offset,
                                         size_t extent,
                                         double startTime,
                                         double duration);

    /** @return a WRITE IOTraceRecord */
    IOTrace::Record* createWriteRecord(int descriptor,
                                       size_t offset,
                                       size_t extent,
                                       double startTime,
                                       double duration);

    /** Trace filename */
    std::string traceFileName_;

    /** Trace input file stream */
    mutable std::ifstream traceFile_;

    // Header data
    int numFiles_;
    int numRecords_;
    int offsetToTraceRecords_;

    /** The next record to retrieve */
    mutable int nextRecord_;
};

#endif

/*
 * Local variables:
 *  indent-tabs-mode: nil
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 *
 * vim: ts=8 sts=4 sw=4 expandtab
 */
