#ifndef PHTF_IO_TRACE_H
#define PHTF_IO_TRACE_H
//
// This file is part of Hecios
//
// Copyright (C) 2008 Yang Wu
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

#include <vector>
#include <map>
#include <string>
#include <sstream>
#include <fstream>
#include <iostream>

class PHTFEvent;
class PHTFEventRecord;
class PHTFArch;
class PHTFFs;


/**
 * PHTF IO Trace Operation Types
 */
enum PHTFOperation{
    INVALID = 0,
    OPEN,   CLOSE,    DELETE,    SET_SIZE,    PREALLOCATE,    GET_SIZE,    GET_GROUP,
    GET_AMODE,    SET_INFO,    GET_INFO,    SET_VIEW,    READ_AT,    READ_AT_ALL,
    WRITE_AT,    WRITE_AT_ALL,    IREAD_AT,    IWRITE_AT,    READ,    READ_ALL,    WRITE,
    WRITE_ALL,    IREAD,    IWRITE,    SEEK,    GET_POSITION,    GET_BYTE_OFFSET,
    READ_SHARED,    WRITE_SHARED,    IREAD_SHARED,    IWRITE_SHARED,    READ_ORDERED,
    WRITE_ORDERED,    SEEK_SHARED,    GET_POSITION_SHARED,    READ_AT_ALL_BEGIN,    READ_AT_ALL_END,
    WRITE_AT_ALL_BEGIN,    WRITE_AT_ALL_END,    READ_ALL_BEGIN,    READ_ALL_END,    WRITE_ALL_BEGIN,
    WRITE_ALL_END,    READ_ORDERED_BEGIN,    READ_ORDERED_END,    WRITE_ORDERED_BEGIN,
    WRITE_ORDERED_END,    GET_TYPE_EXTENT,    SET_ATOMICITY,    GET_ATOMICITY,    SYNC,
    BARRIER, CPU_PHASE, WAIT
    //...                 
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
    std::string recordStr();
    /** Set the record string */
    void recordStr(std::string recordstr);

    /** @return The record id */
    long recordId();
    /** Set the record id */
    void recordId(long recordid);

    /** @return The record operation */
    PHTFOperation recordOp();
    /** Set the operation id */
    void recordOp(PHTFOperation opid);

    /** @return The start time */
    double startTime();
    /** Set the start time */
    void startTime(double sttime);

    /** @return The duration time */
    double duration();
    /** Set the duration time */
    void duration(double duration);

    /** @return The return value */
    long retValue();
    /** Set the return value */
    void retValue(long retvalue);

    /** @return The number of parameters */
    long paraNum();
    /**
     * Set the number of parameters 
     * Only used with paramAt()
     * @see paramAt()
     */
    void paraNum(long paranum);

    /** @return The paraindex-th parameter */
    std::string paramAt(long paraindex);
    /** Set the parameter at position paraindex */
    void paramAt(long paraindex, std::string parastr);

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
    std::vector <std::string> _parameters;
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
    PHTFEvent(std::string filepath);
    /**
     * Constructor
     */
    PHTFEvent() {};
    /**
     * Destrucor
     */
    ~PHTFEvent() {};

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
    PHTFEvent & operator >> (PHTFEventRecord & rec);
    /** Write a record into the event file */
    PHTFEvent & operator << (const PHTFEventRecord & rec);

private:
    std::string _filepath;
    std::ifstream _ifs;
    std::ofstream _ofs;
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
};


/**
 * PHTF IO Trace Handler
 */
class PHTFTrace
{
    static std::string eventFileNamePrefix;

protected:
    /**
     * Constructor
     * @param dirpath String contains the path to the trace directory
     */
    PHTFTrace(std::string dirpath);
    /** Constructor */
    PHTFTrace(){};

public:
    static PHTFTrace * getInstance(std::string dirpath);
    /** Destructor */
    ~PHTFTrace(){destroyEvents();};

public:
    /**
     * Get the event object
     * @param pid The process id
     */
    PHTFEvent * getEvent(long pid);

    /** Build the event object vector */
    void buildEvents();
    void destroyEvents();
    
    /** @return The string that contains the path to the trace directory */
    std::string dirPath();
    /** Set the directory path */
    void dirPath(std::string dirpath);

private:
    std::string _dirpath;
    std::vector<PHTFEvent *> _events;
    PHTFArch _archfile;
    PHTFFs _fsfile;
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
