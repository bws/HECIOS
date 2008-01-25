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

/**
 * Abstract I/O trace class.
 */
class IOTrace
{
public:

    /**
     * Operation Types
     */
    enum Operation {INVALID = 0,
                    OPEN, DELETE, MKDIR, UTIME, STAT,
                    CLOSE, SEEK, READ_AT, READ, WRITE, WRITE_AT};

    /** I/O trace record.  A single entry from the tracefile. */
    class Record
    {
    public:
        /** Constructor without timing info */
        Record(IOTrace::Operation opType, int fileId,
               std::size_t offset, std::size_t length);

        /** Constructor with timing info */
        Record(IOTrace::Operation opType, double timeStamp, double duration);

        /** @return the operation type */
        IOTrace::Operation opType() const {return opType_;};

        /** @return the operation time stamp */
        double timeStamp() const { return timeStamp_; };

        /** @return the operation's duration in seconds */
        double duration() const { return duration_; };
        
        /** @return the file id */
        std::string filename() const { return filename_; };

        /** Set the filename */
        void filename(const std::string& filename) { filename_ = filename; };
        
        /** @return the File id */
        int fileId() const { return fileId_; };
    
        /** Set the file id */
        void fileId(int fileId) { fileId_ = fileId; };
    
        /** @return file access offset */
        std::size_t offset() const { return offset_; };

        /** Set the file access offset */
        void offset(std::size_t offset) { offset_ = offset; };
        
        /** @return file access length */
        std::size_t length() const { return length_; };

        /** Set the file access length */
        void length(std::size_t length) { length_ = length; };

        /** Set the parsed string for debugging purposes */
        void setSource(const std::string& source) { source_ = source; };

        /** @return the source for this trace record */
        std::string source() const { return source_; };
    
    public:
        Operation opType_;
        double timeStamp_;
        double duration_;
        std::string filename_;
        int fileId_;
        std::size_t offset_;
        std::size_t length_;
        std::string source_;
    };

    /** Constructor */
    IOTrace(int numProcs) : numProcs_(numProcs) {};

    /** Destructor */
    virtual ~IOTrace() {};

    /** @return the number of processes for this IO trace */
    int getNumProcs() const {return numProcs_;}

    /** @return true if more records remain */
    virtual bool hasMoreRecords() const = 0;
    
    /** @return the next IOTraceRecord */
    virtual Record* nextRecord() = 0;

    /** Register a filename by file id */
    void addFilename(int fileId, std::string filename);
    
    /** @return the file name for a file id */
    std::string getFilename(int fileId) const;
    
private:

    /** The number of processes for this trace */
    const int numProcs_;

    /** Map unique file ids to a filename */
    std::map<int, std::string> filenamesById_;
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
