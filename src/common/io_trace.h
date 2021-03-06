#ifndef IO_TRACE_H
#define IO_TRACE_H
//
// This file is part of Hecios
//
// Copyright (C) 2007,2008,2009 Brad Settlemyer
// Copyright (C) 2008 Yang Wu
//
// This file is distributed WITHOUT ANY WARRANTY. See the file 'License.txt'
// for details on this and other legal matters.
//
#include <cstddef>
#include <iostream>
#include <map>
#include <string>
#include <omnetpp.h>
#include "pfs_types.h"

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
                    ACCESS, CPU_PHASE, OPEN,
                    MKDIR, UNLINK, UTIME, STAT, CLOSE, SEEK,
                    READ_AT, READ, READDIR, RMDIR, WRITE, WRITE_AT};

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

        /** @return read dir count */
        std::size_t count() const { return count_; };

        /** Set the read dir count */
        void count(std::size_t count) { count_ = count; };

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

        /** @return true if this is a creation */
        bool isCreate() const { return isCreate_; };

        /** Set the create flag */
        void isCreate(bool isCreate) { isCreate_ = isCreate; };

        /** @return true if this is an Exclusive */
        bool isExclusive() const { return isExclusive_; };

        /** Set the Exclusive flag */
        void isExclusive(bool isExclusive) { isExclusive_ = isExclusive; };

        /** @return true if this is a Read Only */
        bool isReadOnly() const { return isReadOnly_; };

        /** Set the Read Only flag */
        void isReadOnly(bool isReadOnly) { isReadOnly_ = isReadOnly; };

        /** @return true if this is a Write Only */
        bool isWriteOnly() const { return isWriteOnly_; };

        /** Set the Write Only flag */
        void isWriteOnly(bool isWriteOnly) { isWriteOnly_ = isWriteOnly; };

        /** @return true if this is a read write */
        bool isReadWrite() const { return isReadWrite_; };

        /** Set the Read Write flag */
        void isReadWrite(bool isReadWrite) { isReadWrite_ = isReadWrite; };

        /** @return true if this is a delete on close operation */
        bool isDeleteOnClose() const { return isDeleteOnClose_; };

        /** Set the Delete On Close flag */
        void isDeleteOnClose(bool isDeleteOnClose) { isDeleteOnClose_ = isDeleteOnClose; };

        /** @return true if this is an Append operation */
        bool isAppend() const { return isAppend_; };

        /** Set the Append flag */
        void isAppend(bool isAppend) { isAppend_ = isAppend; };

        /** @return true if this file exists in the file system */
        bool fileExists() const { return fileExists_; };

        /** Set the file exists flag */
        void fileExists(bool fileExists) { fileExists_ = fileExists; };

    public:
        Operation opType_;
        double timeStamp_;
        double duration_;
        std::string filename_;
        int fileId_;
        std::size_t count_;
        std::size_t offset_;
        std::size_t length_;
        std::string source_;
        bool isCreate_;
        bool isExclusive_;
        bool isReadOnly_;
        bool isWriteOnly_;
        bool isReadWrite_;
        bool isDeleteOnClose_;
        bool isAppend_;
        bool fileExists_;
    };

    /** Constructor */
    IOTrace(int numProcs) : numProcs_(numProcs) {};

    /** Destructor */
    virtual ~IOTrace();

    /** @return the number of processes for this IO trace */
    int getNumProcs() const {return numProcs_;}

    /** @return true if more records remain */
    virtual bool hasMoreRecords() const = 0;

    /** @return the next IOTraceRecord */
    virtual Record* nextRecord() = 0;

    /** Register a directory and number of entries in this traces file system */
    void registerDirectory(const std::string& dirname, std::size_t numEntries);

    /** Register a file and its size that exists in this traces file system */
    void registerFile(const std::string& filename, std::size_t fileSize);

    /** @return a pointer to directories in this trace's file system */
    const FileSystemMap* getDirectories() const;

    /** @return a pointer to files in this trace's file system */
    const FileSystemMap* getFiles() const;

    /** Register a filename by file id */
    void addFilename(int fileId, std::string filename);

    /** @return the file name for a file id */
    std::string getFilename(int fileId) const;

private:

    /** The number of processes for this trace */
    const int numProcs_;

    /** Map unique file ids to a filename */
    std::map<int, std::string> filenamesById_;

    /** Map of all file system directories to their number of entries */
    FileSystemMap entriesByDirectory_;

    /** Map of all file system files to their sizes */
    FileSystemMap fileSizesByName_;
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
