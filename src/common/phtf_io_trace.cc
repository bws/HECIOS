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

#include "phtf_io_trace.h"
using namespace std;

std::map<std::string, PHTFOperation>  PHTFEventRecord::_opmap;
std::string PHTFTrace::eventFileNamePrefix = string("event.");
PHTFTrace* PHTFTrace::_trace = NULL;


PHTFEventRecord::PHTFEventRecord(string recordstr)
{
    recordStr(recordstr);

    buildRecordFields();
    buildRecordStr();
}

PHTFEventRecord::PHTFEventRecord(long reid, PHTFOperation op, double st, double du, long ret, std::vector <std::string> paras)
{
    recordId(reid);
    recordOp(op);
    startTime(st);
    duration(du);
    retValue(ret);
    params(paras);

    buildRecordStr();
}

/** @return The record string */
std::string PHTFEventRecord::recordStr()
{
    return _recordstr;
}

/** Set the record string */
void PHTFEventRecord::recordStr(std::string recordstr)
{
    _recordstr = recordstr;
}

/** @return The record id */
long PHTFEventRecord::recordId()
{
    return _id;
}

/** Set the record id */
void PHTFEventRecord::recordId(long recordid)
{
    _id = recordid;
}

/** @return The record operation */
PHTFOperation PHTFEventRecord::recordOp()
{
    return _opid;
}

/** Set the operation id */
void PHTFEventRecord::recordOp(PHTFOperation opid)
{
    _opid = opid;
}

/** @return The start time */
double PHTFEventRecord::startTime()
{
    return _sttime;
}

/** Set the start time */
void PHTFEventRecord::startTime(double sttime)
{
    _sttime = sttime;
}

/** @return The duration time */
double PHTFEventRecord::duration()
{
    return _duration;
}

/** Set the duration time */
void PHTFEventRecord::duration(double duration)
{
    _duration = duration;
}

/** @return The return value */
long PHTFEventRecord::retValue()
{
    return _ret;
}

/** Set the return value */
void PHTFEventRecord::retValue(long retvalue)
{
    _ret = retvalue;
}

/** @return The number of parameters */
long PHTFEventRecord::paraNum()
{
    return _parameters.size();
}

/**
 * Set the number of parameters 
 * Only used with paramAt()
 * @see paramAt()
 */
void PHTFEventRecord::paraNum(long paranum)
{
    _parameters.resize(paranum);
}

/** @return The paraindex-th parameter */
std::string PHTFEventRecord::paramAt(long paraindex)
{
    if(paraindex < paraNum())
        return (string)_parameters.at(paraindex);
    else
        return "";
}

/** Set the parameter at position paraindex */
void PHTFEventRecord::paramAt(long paraindex, std::string parastr)
{
    if(paraindex >= paraNum())
        _parameters.resize(paraindex + 1);
    _parameters.at(paraindex) = parastr;
}

/** @return The string contains the parameters */
std::string PHTFEventRecord::params()
{
    stringstream ss;
    vector<string>::iterator it;
    for(it = _parameters.begin(); it < _parameters.end(); it ++)
        {
            ss << *it << " ";
        }
    string str = ss.str();
    str.erase(str.end() - 1);
    return str;
}

/** Set the parameters string */
void PHTFEventRecord::params(std::string parastr)
{
    stringstream ss(parastr);
    string pa;

    _parameters.resize(0);
    while(ss.peek() != EOF)
        {
            ss >> pa;
            _parameters.push_back(pa);
        }
}

/** Set the parameter vector */
void PHTFEventRecord::params(std::vector <std::string> paras)
{
    _parameters = paras;
}


/** Initialize the op-to-str map */
void PHTFEventRecord::buildOpMap()
{
    PHTFEventRecord::_opmap["MPI_FILE_OPEN"] = OPEN;
    PHTFEventRecord::_opmap["MPI_FILE_CLOSE"] = CLOSE;
    PHTFEventRecord::_opmap["MPI_FILE_DELETE"] = DELETE;
    PHTFEventRecord::_opmap["MPI_FILE_SET_SIZE"] = SET_SIZE;
    PHTFEventRecord::_opmap["MPI_FILE_PREALLOCATE"] = PREALLOCATE;
    PHTFEventRecord::_opmap["MPI_FILE_GET_SIZE"] = GET_SIZE;
    PHTFEventRecord::_opmap["MPI_FILE_GET_GROUP"] = GET_GROUP;
    PHTFEventRecord::_opmap["MPI_FILE_GET_AMODE"] = GET_AMODE;
    PHTFEventRecord::_opmap["MPI_FILE_SET_INFO"] = SET_INFO;
    PHTFEventRecord::_opmap["MPI_FILE_GET_INFO"] = GET_INFO;
    PHTFEventRecord::_opmap["MPI_FILE_SET_VIEW"] = SET_VIEW;
    PHTFEventRecord::_opmap["MPI_FILE_READ_AT"] = READ_AT;
    PHTFEventRecord::_opmap["MPI_FILE_READ_AT_ALL"] = READ_AT_ALL;
    PHTFEventRecord::_opmap["MPI_FILE_WRITE_AT"] = WRITE_AT;
    PHTFEventRecord::_opmap["MPI_FILE_WRITE_AT_ALL"] = WRITE_AT_ALL;
    PHTFEventRecord::_opmap["MPI_FILE_IREAD_AT"] = IREAD_AT;
    PHTFEventRecord::_opmap["MPI_FILE_IWRITE_AT"] = IWRITE_AT;
    PHTFEventRecord::_opmap["MPI_FILE_READ"] = READ;
    PHTFEventRecord::_opmap["MPI_FILE_READ_ALL"] = READ_ALL;
    PHTFEventRecord::_opmap["MPI_FILE_WRITE"] = WRITE;
    PHTFEventRecord::_opmap["MPI_FILE_WRITE_ALL"] = WRITE_ALL;
    PHTFEventRecord::_opmap["MPI_FILE_IREAD"] = IREAD;
    PHTFEventRecord::_opmap["MPI_FILE_IWRITE"] = IWRITE;
    PHTFEventRecord::_opmap["MPI_FILE_SEEK"] = SEEK;
    PHTFEventRecord::_opmap["MPI_FILE_GET_POSITION"] = GET_POSITION;
    PHTFEventRecord::_opmap["MPI_FILE_GET_BYTE_OFFSET"] = GET_BYTE_OFFSET;
    PHTFEventRecord::_opmap["MPI_FILE_READ_SHARED"] = READ_SHARED;
    PHTFEventRecord::_opmap["MPI_FILE_WRITE_SHARED"] = WRITE_SHARED;
    PHTFEventRecord::_opmap["MPI_FILE_IREAD_SHARED"] = IREAD_SHARED;
    PHTFEventRecord::_opmap["MPI_FILE_IWRITE_SHARED"] = IWRITE_SHARED;
    PHTFEventRecord::_opmap["MPI_FILE_READ_ORDERED"] = READ_ORDERED;
    PHTFEventRecord::_opmap["MPI_FILE_WRITE_ORDERED"] = WRITE_ORDERED;
    PHTFEventRecord::_opmap["MPI_FILE_SEEK_SHARED"] = SEEK_SHARED;
    PHTFEventRecord::_opmap["MPI_FILE_GET_POSITION_SHARED"] = GET_POSITION_SHARED;
    PHTFEventRecord::_opmap["MPI_FILE_READ_AT_ALL_BEGIN"] = READ_AT_ALL_BEGIN;
    PHTFEventRecord::_opmap["MPI_FILE_READ_AT_ALL_END"] = READ_AT_ALL_END;
    PHTFEventRecord::_opmap["MPI_FILE_WRITE_AT_ALL_BEGIN"] = WRITE_AT_ALL_BEGIN;
    PHTFEventRecord::_opmap["MPI_FILE_WRITE_AT_ALL_END"] = WRITE_AT_ALL_END;
    PHTFEventRecord::_opmap["MPI_FILE_READ_ALL_BEGIN"] = READ_ALL_BEGIN;
    PHTFEventRecord::_opmap["MPI_FILE_READ_ALL_END"] = READ_ALL_END;
    PHTFEventRecord::_opmap["MPI_FILE_WRITE_ALL_BEGIN"] = WRITE_ALL_BEGIN;
    PHTFEventRecord::_opmap["MPI_FILE_WRITE_ALL_END"] = WRITE_ALL_END;
    PHTFEventRecord::_opmap["MPI_FILE_READ_ORDERED_BEGIN"] = READ_ORDERED_BEGIN;
    PHTFEventRecord::_opmap["MPI_FILE_READ_ORDERED_END"] = READ_ORDERED_END;
    PHTFEventRecord::_opmap["MPI_FILE_WRITE_ORDERED_BEGIN"] = WRITE_ORDERED_BEGIN;
    PHTFEventRecord::_opmap["MPI_FILE_WRITE_ORDERED_END"] = WRITE_ORDERED_END;
    PHTFEventRecord::_opmap["MPI_FILE_GET_TYPE_EXTENT"] = GET_TYPE_EXTENT;
    PHTFEventRecord::_opmap["MPI_FILE_SET_ATOMICITY"] = SET_ATOMICITY;
    PHTFEventRecord::_opmap["MPI_FILE_GET_ATOMICITY"] = GET_ATOMICITY;
    PHTFEventRecord::_opmap["MPI_FILE_SYNC"] = SYNC;
    PHTFEventRecord::_opmap["MPI_FILE_READ"] = READ;
    PHTFEventRecord::_opmap["MPI_FILE_WRITE"] = WRITE;
    PHTFEventRecord::_opmap["MPI_BARRIER"] = BARRIER;
    PHTFEventRecord::_opmap["CPU_PHASE"] = CPU_PHASE;
    PHTFEventRecord::_opmap["MPIO_WAIT"] = WAIT;
}

/** @return the operation id */
PHTFOperation PHTFEventRecord::strToOp(std::string opstr)
{
    string op = opstr;
    for(string::iterator it = op.begin(); it != op.end(); it++)
        *it = toupper(*it);
    if(PHTFEventRecord::_opmap.count(op) > 0)return PHTFEventRecord::_opmap.find(op)->second;
    else return INVALID;
}

/** @return the operation string */
std::string PHTFEventRecord::opToStr(PHTFOperation opid)
{
    map<string, PHTFOperation>::iterator it;
    for(it = PHTFEventRecord::_opmap.begin(); it != PHTFEventRecord::_opmap.end(); it ++)
        {
            if(it->second == opid)
                return it->first;
        }
    return "INVALID";
}

/** Build the record string */
void PHTFEventRecord::buildRecordStr()
{
    stringstream ss;
    
    ss << _id << " " << opToStr(_opid) << " " << _sttime << " "
       << _duration << " " << _ret << " ";

    vector<string>::iterator it;
    for(it = _parameters.begin(); it < _parameters.end(); it ++)
        {
            ss << *it << " ";
        }
    _recordstr = ss.str();
    _recordstr.erase(_recordstr.end() - 1);
}

/** Build the record fields */
void PHTFEventRecord::buildRecordFields()
{
    stringstream ss(_recordstr);
    string opstr;
    string pa;

    ss >> _id >> opstr >> _sttime >> _duration >> _ret;

    _parameters.clear();
    while(ss.peek() != EOF)
        {
            ss >> pa;
            _parameters.push_back(pa);
        }

    _opid = strToOp(opstr);
}

/**
 * Constructor
 * @param filepath The path to the event file
 */
PHTFEvent::PHTFEvent(string filepath)
{
    filePath(filepath);
}

/** @return The string that contains the file path */
std::string PHTFEvent::filePath()
{
    return _filepath;
}

/** Set the path to the event file */
void PHTFEvent::filePath(std::string filepath)
{
    _filepath = filepath;
}

/** @return Whether event file has reached the end */
bool PHTFEvent::eof()
{
    return _ifs.eof();
}

/** 
 * Open the event file
 * @param write Whther open in write mode
 * @return -1 if failed, 0 if success
 */
int PHTFEvent::open(bool write)
{
    if(write)
        {
            if(_ofs.is_open())_ofs.close();
            _ofs.open(_filepath.c_str());
            if(_ofs.is_open())return 0;
            else return -1;
        }
    else
        {
            if(_ifs.is_open())_ifs.close();
            _ifs.open(_filepath.c_str());
            if(_ifs.is_open())return 0;
            else return -1;
        }
}

/** Close the event file */
void PHTFEvent::close()
{
    if(_ofs.is_open())_ofs.close();
    if(_ifs.is_open())_ifs.close();
}

/** Extract a record from the event file */
PHTFEvent & PHTFEvent::operator >> (PHTFEventRecord & rec)
{
    string line;
    if(_ifs.is_open())
        {
            getline(_ifs, line);
            rec = PHTFEventRecord(line);
        }
    return *this;
}

/** Write a record into the event file */
PHTFEvent & PHTFEvent::operator << (const PHTFEventRecord & rec)
{
    return *this;
}

/**
 * Constructor
 * @param dirpath String contains the path to the trace directory
 */
PHTFTrace::PHTFTrace(std::string dirpath)
{
    dirPath(dirpath);
    buildEvents();
}

/** Build the event object vector */
void PHTFTrace::buildEvents()
{
    int size = 32; // number of events files, temporily use 32, should be implemented in PHTFArch
    
    destroyEvents();
    for(int i = 0; i < size; i ++)
        {
            stringstream ss("");
            ss << dirPath() << PHTFTrace::eventFileNamePrefix << i;
            PHTFEvent *ev = new PHTFEvent(ss.str());
            _events.push_back(ev); 
        }
}

void PHTFTrace::destroyEvents()
{
    int size = _events.size();
    for(int i = 0; i < size; i ++)
        {
            delete _events.at(i);
        }
    _events.clear();
}

/**
 * Get the event object
 * @param pid The process id
 */
PHTFEvent * PHTFTrace::getEvent(long pid)
{
    if(pid < (long)_events.size())
        {
            return _events.at(pid);
        }
    else
        {
            return (PHTFEvent *)NULL;
        }
}


/** @return The string that contains the path to the trace directory */
std::string PHTFTrace::dirPath()
{
    return _dirpath;
}

/** Set the directory path */
void PHTFTrace::dirPath(std::string dirpath)
{
    _dirpath = dirpath;
}

PHTFTrace* PHTFTrace::getInstance(string dirpath)
{
    if(!PHTFTrace::_trace)
        {
            PHTFTrace::_trace = new PHTFTrace(dirpath);
        }
    else
        {
            if(dirpath.compare(PHTFTrace::_trace->dirPath()))
                {
                    PHTFTrace::_trace->dirPath(dirpath);
                    PHTFTrace::_trace->buildEvents();
                }
        }
    return PHTFTrace::_trace;
}


/*
 * Local variables:
 *  indent-tabs-mode: nil
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 *
 * vim: ts=8 sts=4 sw=4 expandtab
 */
