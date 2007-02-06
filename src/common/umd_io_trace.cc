#include "umd_io_trace.h"
//using namespace std;

/**
 * Construct the file names from a single trace file
 */
UMDIOTrace::UMDIOTrace(int numProcs, std::string traceFile)
    : IOTrace(numProcs)
{
    traceFile_ = traceFile;
}

/**
 * Construct the next record from the trace and return a pointer to it
 */
IOTraceRecord* UMDIOTrace::nextRecord() const
{
    return 0;
}

/*
 * Local variables:
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 *
 * vim: ts=8 sts=4 sw=4 expandtab
 */
