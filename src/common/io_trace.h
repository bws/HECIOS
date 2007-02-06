#ifndef IO_TRACE_H
#define IO_TRACE_H

#include <iostream>

/** Forward declarations */
class IOTraceRecord;

/**
 * @class Abstract I/O trace class
 */
class IOTrace
{
public:

    /**
     * Operation Types
     */
    enum operation {NOOP = 0,
                    OPEN, CLOSE,
                    DELETE, SEEK,
                    READ_AT, READ,
                    WRITE, WRITE_AT};

    /** Constructor */
    IOTrace(int numProcs) : _numProcs(numProcs) {};

    /** Destructor */
    virtual ~IOTrace() {};

    /** @return the number of processes for this IO trace */
    int getNumProcs() const {return _numProcs;}

    /** @return the next IOTraceRecord */
    virtual IOTraceRecord* nextRecord() const = 0;

private:

    /** The number of processes for this trace */
    const int _numProcs;
};

/**
 * @class Individual I/O trace record
 */
class IOTraceRecord
{
public:
    /** Constructor */
    IOTraceRecord(int rank, IOTrace::operation opType) :
        _rank(rank), _opType(opType) {};

    /** Destructor */
    virtual ~IOTraceRecord() {};

    /** Operation type getter */
    IOTrace::operation getOpType() const {return _opType;};

    /** File offset getter */
    size_t getOffset() const {return _offset;};

    /** Length getter */
    size_t getLength() const {return _length;};
    
private:

    int _rank;
    IOTrace::operation _opType;
    size_t _offset;
    size_t _length;
    
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
