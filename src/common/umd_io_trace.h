#ifndef UMD_IO_TRACE_H
#define UMD_IO_TRACE_H

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
    virtual IOTraceRecord* nextRecord();

    /** @return the next IOTraceRecord as a cMessage */
    virtual cMessage* nextRecordAsMessage();

private:

    enum OpType {OPEN = 0, CLOSE = 1, READ = 2,
                 WRITE = 3, SEEK = 4, LISTIO_HEADER = 5};

    /** @return the next IOTraceRecord as a cMessage */
    cMessage* createMPIIOMessage(OpType opType, int fileId,
                                 long offset, long length);
    
    std::string traceFileName_;
    mutable std::ifstream traceFile_;

    // Header data
    int numProcs_;
    int numFiles_;
    int numRecords_;
    int offsetToTraceRecords_;
    
    std::string* fileNames_;

    mutable int curRecord_;
};

#endif

/*
 * Local variables:
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 *
 * vim: ts=8 sts=4 sw=4 expandtab
 */
