#ifndef UMD_IO_TRACE_H
#define UMD_IO_TRACE_H

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
    UMDIOTrace(int numProcs, std::string traceFile);

    /** Destructor */
    virtual ~UMDIOTrace() {};

    /** @return the next IOTraceRecord */
    virtual IOTraceRecord* nextRecord() const;

private:
    std::string traceFile_;
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
