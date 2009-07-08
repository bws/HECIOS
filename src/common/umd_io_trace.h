#ifndef UMD_IO_TRACE_H
#define UMD_IO_TRACE_H
//
// This file is part of Hecios
//
// Copyright (C) 2007,2008,2009 Brad Settlemyer
//
// This file is distributed WITHOUT ANY WARRANTY. See the file 'License.txt'
// for details on this and other legal matters.
//
#include <fstream>
#include <string>
#include "io_trace.h"

/**
 * An I/O trace in the University of Maryland format.  Traces are available
 * at the following url: http://www.cs.umd.edu/xxx
 */
class UMDIOTrace: public IOTrace
{
public:

    /** Constructor */
    UMDIOTrace(int numProcs, std::string traceFileName);

    /** Destructor */
    virtual ~UMDIOTrace();

    /** @return true if the trace file is valid */
    bool isValid() const {return traceFile_;};

    /** @return true if more records remain */
    bool hasMoreRecords() const { return (curRecord_ < numRecords_); };

    /** @return the next IOTraceRecord */
    virtual IOTrace::Record* nextRecord();

private:

    enum OpType {OPEN = 0, CLOSE = 1, READ = 2,
                 WRITE = 3, SEEK = 4, LISTIO_HEADER = 5};

    /** @return the next IOTraceRecord */
    IOTrace::Record* createIOTraceRecord(OpType opType, int fileId,
                                         long offset, long length);

    std::string traceFileName_;
    mutable std::ifstream traceFile_;

    // Header data
    int numProcs_;
    int numFiles_;
    int numRecords_;
    int offsetToTraceRecords_;

    mutable int curRecord_;
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
