#ifndef IO_TRACE_H
#define IO_TRACE_H

#include <iostream>
#include <map>
#include <omnetpp.h>

/** Forward declarations */
class FSOpenFile;
class IOTraceRecord;

/**
 * Abstract I/O trace class.
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
    IOTrace(int numProcs) : numProcs_(numProcs) {};

    /** Destructor */
    virtual ~IOTrace() {};

    /** @return the number of processes for this IO trace */
    int getNumProcs() const {return numProcs_;}

    /** @return true if more records remain */
    virtual bool hasMoreRecords() const = 0;
    
    /** @return the next IOTraceRecord */
    virtual IOTraceRecord* nextRecord() = 0;

    /** @return the next IOTraceRecord */
    virtual cMessage* nextRecordAsMessage() = 0;

protected:

    /** */
    void setDescriptor(int fileId, FSOpenFile* descriptor);

    /** */
    FSOpenFile* getDescriptor(int fileId) const;
    
private:

    /** The number of processes for this trace */
    const int numProcs_;

    /** */
    std::map<int, FSOpenFile*> descriptorById_;
};

/**
 * I/O trace record.  A single entry from the tracefile.
 */
class IOTraceRecord
{
public:
    /** Constructor */
    IOTraceRecord(int rank, IOTrace::operation opType) :
        rank_(rank), opType_(opType) {};

    /** Destructor */
    virtual ~IOTraceRecord() {};

    /** Operation type getter */
    IOTrace::operation getOpType() const {return opType_;};

    /** File offset getter */
    size_t getOffset() const {return offset_;};

    /** Length getter */
    size_t getLength() const {return length_;};
    
private:

    int rank_;
    IOTrace::operation opType_;
    size_t offset_;
    size_t length_;
    
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
