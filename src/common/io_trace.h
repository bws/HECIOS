#ifndef IO_TRACE_H
#define IO_TRACE_H

#include <iostream>
#include <map>
#include <omnetpp.h>

/** Forward declarations */
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

    /** Register a filename by file id */
    void addFilename(int fileId, std::string filename);
    
    /** @return the file name for a file id */
    std::string getFilename(int fileId) const;
    
private:

    /** The number of processes for this trace */
    const int numProcs_;

    /** */
    std::map<int, std::string> filenamesById_;
};

/**
 * I/O trace record.  A single entry from the tracefile.
 */
class IOTraceRecord
{
public:

    /** Constructor */
    IOTraceRecord(IOTrace::operation opType, int fileId,
                  size_t offset, size_t length) :
        opType_(opType), fileId_(fileId), offset_(offset), length_(length) {};

    /** Destructor */
    virtual ~IOTraceRecord() {};

    /** Operation type getter */
    IOTrace::operation opType() const {return opType_;};

    /** File id getter */
    int fileId() const {return fileId_;};
    
    /** File offset getter */
    size_t offset() const {return offset_;};

    /** Length getter */
    size_t length() const {return length_;};
    
private:

    IOTrace::operation opType_;
    int fileId_;
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
