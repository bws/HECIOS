#ifndef PHTF_IO_TRACE_H
#define PHTF_IO_TRACE_H
//
// This file is part of Hecios
//
// Copyright (C) 2008 Yang Wu
// Copyright (C) 2008,2009 Brad Settlemyer
//
// This file is distributed WITHOUT ANY WARRANTY. See the file 'License.txt'
// for details on this and other legal matters.
//
#include <cstddef>
#include <stdint.h>
#include <vector>
#include <map>
#include <string>
#include <sstream>
#include <fstream>
#include <iostream>
#include "singleton.h"
class PHTFEvent;
class PHTFEventRecord;
class PHTFArch;
class PHTFFs;
class PHTFIni;


/**
 * PHTF IO Trace Operation Types
 */
enum PHTFOperation{
    INVALID = 0,
    ALLREDUCE, BARRIER, BCAST, CPU_PHASE,
    CART_CREATE, CART_GET,
    COMM_CREATE, COMM_DUP, COMM_RANK, COMM_SPLIT,
    OPEN,   CLOSE,    DELETE,    SET_SIZE,    PREALLOCATE,    GET_SIZE,    GET_GROUP,
    GET_AMODE,    SET_INFO,    GET_INFO,    SET_VIEW,    READ_AT,    READ_AT_ALL,
    WRITE_AT,    WRITE_AT_ALL,    IREAD_AT,    IWRITE_AT,    READ,    READ_ALL,    WRITE,
    WRITE_ALL,    IREAD,    IWRITE,    SEEK,    GET_POSITION,    GET_BYTE_OFFSET,
    READ_SHARED,    WRITE_SHARED,    IREAD_SHARED,    IWRITE_SHARED,    READ_ORDERED,
    WRITE_ORDERED,    SEEK_SHARED,    GET_POSITION_SHARED,    READ_AT_ALL_BEGIN,    READ_AT_ALL_END,
    WRITE_AT_ALL_BEGIN,    WRITE_AT_ALL_END,    READ_ALL_BEGIN,    READ_ALL_END,    WRITE_ALL_BEGIN,
    WRITE_ALL_END,    READ_ORDERED_BEGIN,    READ_ORDERED_END,    WRITE_ORDERED_BEGIN,
    WRITE_ORDERED_END,    GET_TYPE_EXTENT,    SET_ATOMICITY,    GET_ATOMICITY,    SYNC,
    TYPE_CONTIGUOUS, TYPE_STRUCT, TYPE_CREATE_SUBARRAY, TYPE_VECTOR, TYPE_COMMIT,
    WAIT,
    //...
};

enum PHTFDataType{
    BASIC = 0,
    CONTIGUOUS,
    STRUCT,
    SUBARRAY,
    VECTOR
};

/**
 * PHTF IO Trace Event Record Handler
 */
class PHTFEventRecord
{
    static std::map<std::string, PHTFOperation> _opmap;
public:
    /**
     * Constructor
     * @param recordstr A string contains the record
     */
    PHTFEventRecord(std::string recordstr);

    /**
     * Constructor
     * @param reid The record id
     * @param op The operation id
     * @param st The start time
     * @param du The duration time
     * @param ret The return value
     * @param paras The parameter vector
     */
    PHTFEventRecord(long reid, PHTFOperation op, double st, double du, long ret, std::vector <std::string> paras);

    /**
     * Constructor
     */
    PHTFEventRecord(){};

    /**
     * Destructor
     */
    ~PHTFEventRecord(){};

    /** @return The record string */
    std::string recordStr() const;
    /** Set the record string */
    void recordStr(std::string recordstr);

    /** @return The record id */
    long recordId() const;
    /** Set the record id */
    void recordId(long recordid);

    /** @return The record operation */
    PHTFOperation recordOp() const;
    /** Set the operation id */
    void recordOp(PHTFOperation opid);

    /** @return The start time */
    double startTime() const;
    /** Set the start time */
    void startTime(double sttime);

    /** @return The duration time */
    double duration() const;
    /** Set the duration time */
    void duration(double duration);

    /** @return The return value */
    long retValue() const;
    /** Set the return value */
    void retValue(long retvalue);

    /**
     * Set the number of parameters
     * Only used with paramAt()
     * @see paramAt()
     */
    void paraNum(long paranum);

    /** Set the parameter at position paraindex */
    void paramAt(std::size_t idx, std::string parastr);

    /** @return The number of parameters */
    std::size_t paraNum() const;

    /** @return The paraindex-th parameter */
    std::string paramAt(std::size_t idx) const;

    /** @return The paramindex-th parameter as a base16 address */
    uint64_t paramAsAddress(std::size_t idx) const;

    /** @return The paramindex-th parameter as a size_t */
    std::size_t paramAsSizeT(std::size_t idx) const;

    /** @return The paramindex-th parameter as a file descriptor */
    int paramAsDescriptor(std::size_t paramindex, const PHTFEvent & event) const;

    /** @return The paramindex-th parameter as a file name */
    std::string paramAsFilename(std::size_t paramindex, const PHTFEvent & event) const;

    /** @return the idx-th parameter as a vector of size_t */
    std::vector<size_t> paramAsVector(std::size_t idx) const;

    /** @return The string contains the parameters */
    std::string params();
    /** Set the parameters string */
    void params(std::string parastr);
    /** Set the parameter vector */
    void params(std::vector <std::string> paras);

    /** Initialize the op-to-str map, call it before using RecordEvent */
    static void buildOpMap();

    /** @return the operation id */
    PHTFOperation strToOp(std::string opstr);
    /** @return the operation string */
    std::string opToStr(PHTFOperation opid);

    /** Build the record string */
    void buildRecordStr();

    /** Build the record fields */
    void buildRecordFields();

private:
    std::string _recordstr;
    long _id;
    PHTFOperation _opid;
    double _sttime;
    double _duration;
    long _ret;
    std::vector<std::string> _parameters;
};

/**
 * PHTF IO Trace Event File Handler (Single File)
 */
class PHTFEvent
{
public:
    /**
     * Constructor
     * @param filepath The path to the event file
     */
    PHTFEvent(const std::string& filepath);

    /** Destructor */
    ~PHTFEvent();

    /** @return The string that contains the file path */
    std::string filePath();
    /** Set the path to the event file */
    void filePath(std::string filepath);

    /** @return Whether event file has reached the end */
    bool eof();
    /**
     * Open the event file
     * @param write Whther open in write mode
     * @return -1 if failed, 0 if success
     */
    int open(bool write = false);
    /** Close the event file */
    void close();

    /** Extract a record from the event file */
    PHTFEvent& operator>>(PHTFEventRecord& rec);

    /** Write a record into the event file */
    PHTFEvent& operator<<(const PHTFEventRecord& rec);

private:
    std::string _filepath;
    std::fstream file_;
};

/**
 * PHTF Ini File Handler
 */
typedef std::map<std::string, std::string> PHTFIniItem;

class PHTFIni
{
protected:
    std::map<std::string, PHTFIniItem *> data_;
    void readIni();
    void writeIni();

private:
    std::string fileName_;
    bool isWriteOnly_;

public:
    PHTFIni(const std::string& filename);
    ~PHTFIni();

    bool exist(std::string section);
    bool exist(std::string section, std::string field);
    std::string iniValue(std::string section, std::string field);
    void iniValue(std::string section, std::string field, std::string value);
    PHTFIniItem * iniSection(std::string section);

    void init(bool write);
    void clear();
};

/**
 * PHTF IO Trace Arch File Handler
 */
class PHTFArch
{

};

/**
 * PHTF IO Trace FS File Handler
 */
class PHTFFs
{
public:

    static std::string fsSecName;
    static std::string fsConst;

    PHTFFs(std::string filepath, bool write);
    PHTFFs(std::string filepath);
    PHTFFs(){};
    ~PHTFFs(){};

    void addFile(std::string filename, std::string filesize);
    std::string fileName(int id);
    int fileSize(int id);
    int fileSize(std::string filename);
    int fileNum();

    std::string consts(std::string constName);
    void consts(std::string constName, std::string constValue);

protected:
    PHTFIniItem::iterator item(int id);
private:
    PHTFIni *_fsini;
};


/**
 * PHTF IO Trace Handler
 */
class PHTFTrace : public Singleton<PHTFTrace>
{
public:
    /** Enable singleton construction */
    friend class Singleton<PHTFTrace>;

    /** Destructor */
    ~PHTFTrace() {destroyEvents(); delete _fsfile; };

    /**
     * Get the event object
     * @param pid The process id
     */
    PHTFEvent * getEvent(long pid);

    PHTFFs *getFs();

    /** Build the event object vector */
    void destroyEvents();

    /** @return The string that contains the path to the trace directory */
    std::string dirPath();
    /** Set the directory path */
    void dirPath(std::string dirpath);

protected:
    /** Constructor */
    PHTFTrace();

private:
    static std::string eventFileNamePrefix;
    static std::string fsFileName;

    std::string _dirpath;
    std::vector<PHTFEvent *> _events;
    PHTFArch *_archfile;
    PHTFFs *_fsfile;
    static PHTFTrace *_trace;
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
