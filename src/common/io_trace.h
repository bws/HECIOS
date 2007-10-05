#ifndef IO_TRACE_H
#define IO_TRACE_H
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
#include <cstddef>
#include <iostream>
#include <map>
#include <string>
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
                  std::size_t offset, std::size_t length) :
        opType_(opType), fileId_(fileId), offset_(offset), length_(length) {};

    /** Destructor */
    virtual ~IOTraceRecord() {};

    /** Operation type getter */
    IOTrace::operation opType() const {return opType_;};

    /** File id getter */
    int fileId() const {return fileId_;};
    
    /** File offset getter */
    std::size_t offset() const {return offset_;};

    /** Length getter */
    std::size_t length() const {return length_;};

    /** Set the parsed string for debugging purposes */
    void setSource(const std::string& source) {source_ = source;};

    /** @return the source for this trace record */
    std::string source() const {return source_;};
    
private:

    IOTrace::operation opType_;
    int fileId_;
    std::size_t offset_;
    std::size_t length_;
    std::string source_;
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
