//
// This file is part of Hecios
//
// Copyright (C) 2008 Bradley W. Settlemyer
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
#include <cassert>
#include <cstdlib>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <regex.h>
#include <sstream>
#include <sys/types.h>
#include "lanl_trace_scan_actions.h"
using namespace std;

static struct timespec diff_timespec(struct timespec btime, struct timespec etime)
{
    struct timespec result;
    // Perfomr the carry for later subtraction
    if (etime.tv_nsec < btime.tv_nsec) {
        long num_secs = (btime.tv_nsec - etime.tv_nsec) / 1000000000 + 1;
        btime.tv_nsec -= 1000000000 * num_secs;
        btime.tv_sec += num_secs;
    }

    if (etime.tv_nsec - btime.tv_nsec > 1000000000) {
        long num_secs = (etime.tv_nsec - btime.tv_nsec) / 1000000000;
        btime.tv_nsec += 1000000000 * num_secs;
        btime.tv_sec -= num_secs;
    }

    result.tv_sec = etime.tv_sec - btime.tv_sec;
    result.tv_nsec = etime.tv_nsec - btime.tv_nsec;
    return result;
}

void handleMPIAllReduce(const char* text)
{
    LanlTraceScanActions::instance().handleMPIAllReduce(text);
}

void handleMPIBarrier(const char* text)
{
    LanlTraceScanActions::instance().handleMPIBarrier(text);
}

void handleMPIBcast(const char* text)
{
    LanlTraceScanActions::instance().handleMPIBcast(text);
}

void handleMPICommCreate(const char* text)
{
    LanlTraceScanActions::instance().handleMPICommCreate(text);
}

void handleMPICommCompare(const char* text)
{
    LanlTraceScanActions::instance().handleMPICommCompare(text);
}

void handleMPICommDup(const char* text)
{
    LanlTraceScanActions::instance().handleMPICommDup(text);
}

void handleMPICommFree(const char* text)
{
    LanlTraceScanActions::instance().handleMPICommFree(text);
}

void handleMPICommRank(const char* text)
{
    LanlTraceScanActions::instance().handleMPICommRank(text);
}

void handleMPICommSize(const char* text)
{
    LanlTraceScanActions::instance().handleMPICommSize(text);
}

void handleMPICommSplit(const char* text)
{
    LanlTraceScanActions::instance().handleMPICommSplit(text);
}

void handleMPIFileClose(const char* text)
{
    LanlTraceScanActions::instance().handleMPIFileClose(text);
}

void handleMPIFileDelete(const char* text)
{
    LanlTraceScanActions::instance().handleMPIFileDelete(text);
}

void handleMPIFileGetAMode(const char* text)
{
    LanlTraceScanActions::instance().handleMPIFileGetAMode(text);
}

void handleMPIFileGetGroup(const char* text)
{
    LanlTraceScanActions::instance().handleMPIFileGetGroup(text);
}

void handleMPIFileGetSize(const char* text)
{
    LanlTraceScanActions::instance().handleMPIFileGetSize(text);
}

void handleMPIFileOpen(const char* text)
{
    LanlTraceScanActions::instance().handleMPIFileOpen(text);
}

void handleMPIFileRead(const char* text)
{
    LanlTraceScanActions::instance().handleMPIFileRead(text);
}

void handleMPIFileReadAll(const char* text)
{
    LanlTraceScanActions::instance().handleMPIFileReadAll(text);
}

void handleMPIFileReadAt(const char* text)
{
    LanlTraceScanActions::instance().handleMPIFileReadAt(text);
}

void handleMPIFileReadAtAll(const char* text)
{
    LanlTraceScanActions::instance().handleMPIFileReadAtAll(text);
}

void handleMPIFileSeek(const char* text)
{
    LanlTraceScanActions::instance().handleMPIFileSeek(text);
}

void handleMPIFileSetSize(const char* text)
{
    LanlTraceScanActions::instance().handleMPIFileSetSize(text);
}

void handleMPIFileWrite(const char* text)
{
    LanlTraceScanActions::instance().handleMPIFileWrite(text);
}

void handleMPIFileWriteAll(const char* text)
{
    LanlTraceScanActions::instance().handleMPIFileWriteAll(text);
}

void handleMPIFileWriteAt(const char* text)
{
    LanlTraceScanActions::instance().handleMPIFileWriteAt(text);
}

void handleMPIFileWriteAtAll(const char* text)
{
    LanlTraceScanActions::instance().handleMPIFileWriteAtAll(text);
}

void handleMPIFinalize(const char* text)
{
    LanlTraceScanActions::instance().handleMPIFinalize(text);
}

void handleMPIGetElements(const char* text)
{
    LanlTraceScanActions::instance().handleMPIGetElements(text);
}

void handleMPIGetProcessorName(const char* text)
{
    LanlTraceScanActions::instance().handleMPIGetProcessorName(text);
}

void handleMPIInfoCreate(const char* text)
{
    LanlTraceScanActions::instance().handleMPIInfoCreate(text);
}

void handleMPIInfoDup(const char* text)
{
    LanlTraceScanActions::instance().handleMPIInfoDup(text);
}

void handleMPIInfoFree(const char* text)
{
    LanlTraceScanActions::instance().handleMPIInfoFree(text);
}

void handleMPIInit(const char* text)
{
    LanlTraceScanActions::instance().handleMPIInit(text);
}

void handleMPITypeSize(const char* text)
{
    LanlTraceScanActions::instance().handleMPITypeSize(text);
}

void handleMPIWTime(const char* text)
{
    LanlTraceScanActions::instance().handleMPIWTime(text);
}

/* Static data initialization */
const string LanlTraceScanActions::CPU_PHASE_CALLNAME = "CPU_PHASE";
size_t LanlTraceScanActions::currentTraceCallId_ = 0;

LanlTraceScanActions::LanlTraceScanActions()
    : currentFile_(make_pair("", 0)),
      nextFileDescriptor_(100)
{
    memset(&epochTime_, 0, sizeof(timespec));
}

LanlTraceScanActions::~LanlTraceScanActions()
{
}

void LanlTraceScanActions::setEpochTime(const string& timestamp)
{
    cerr << "Setting epoch time to: " << timestamp << endl;
    epochTime_ = createTime(timestamp);
}

void LanlTraceScanActions::emitTraceCalls(ostream& ost)
{
    size_t id = 0;
    double prevCompleteTime = 0.0;
    for (size_t i = 0; i < traceCalls_.size(); i++)
    {
        TraceCall currentCall = traceCalls_[i];
        double invokeTime = currentCall.invokeTime;
        if ((invokeTime - prevCompleteTime) >= EPSILON)
        {
            TraceCall cpuPhase = createCpuPhase(prevCompleteTime,
                                                invokeTime - prevCompleteTime);
            // Rewrite id numbers during output
            cpuPhase.id = id++;
            ost << cpuPhase << endl;
        }
        // Rewrite id numbers during output
        currentCall.id = id++;
        ost << currentCall << endl;

        // Update the previous completion time
        prevCompleteTime = currentCall.invokeTime + currentCall.duration;
    }
}

void LanlTraceScanActions::handleMPIAllReduce(const string& text)
{
    //cerr << __FILE__ << ":" << __LINE__ << ":" << "Handling MPI_ALLREDUCE" << endl;
    //cerr << "Allreduce Text: " << text << endl;
    // Extract the start time, duration
    string startTime = extractStartTime(text);
    string duration = extractDuration(text);

    // Extract send buffer address
    size_t begin = text.find("MPI_Allreduce(") + strlen("MPI_Allreduce(");
    size_t end = text.find(",", begin);
    string sendAddr = text.substr(begin, end - begin);

    // Extract receive buffer address
    begin = end + 2;
    end = text.find(",", begin);
    string recvAddr = text.substr(begin, end - begin);

    // Extract the count
    begin = end + 2;
    end = text.find(",", begin);
    string count = text.substr(begin, end - begin);

    // Extract the data type id
    begin = end + 2;
    end = text.find(",", begin);
    string dataType = text.substr(begin, end - begin);

    // Extract operation type
    begin = end + 2;
    end = text.find(",", begin);
    string opType = text.substr(begin, end - begin);

    // Extract the communicator id
    begin = end + 2;
    end = text.find(" ", begin);
    string commId = text.substr(begin, end - begin);

    TraceCall tc = createTraceCall("MPI_ALLREDUCE", startTime, duration);
    tc.params.push_back(sendAddr);
    tc.params.push_back(recvAddr);
    tc.params.push_back(count);
    tc.params.push_back(dataType);
    tc.params.push_back(opType);
    tc.params.push_back(commId);
    traceCalls_.push_back(tc);
}

void LanlTraceScanActions::handleMPIBarrier(const string& text)
{
    //cerr << __FILE__ << ":" << __LINE__ << ":" << "Handling MPI_Barrier" << endl;
    //cerr << "Barrier Text: " << text << endl;

    // Extract the start time, duration, and communicator id
    string startTime = extractStartTime(text);
    string duration = extractDuration(text);
    size_t pos = text.find("MPI_Barrier(");
    string comm = text.substr(pos + 12, 10);

    TraceCall tc = createTraceCall("MPI_BARRIER", startTime, duration);
    tc.params.push_back(comm);
    traceCalls_.push_back(tc);
}

void LanlTraceScanActions::handleMPIBcast(const string& text)
{
    //cerr << __FILE__ << ":" << __LINE__ << ":" << "Handling MPI_BCAST" << endl;
    //cerr << "Bcast Text: " << text << endl;

    // Extract the start time and duration
    string startTime = extractStartTime(text);
    // TODO: This definitely needs to be fixed ASAP
    //string duration = extractDuration(text);
    string duration = "0.0";

    // Extract the buffer address
    size_t begin = text.find("MPI_Bcast(") + 10;
    size_t end = text.find(",", begin);
    string bufferAddr = text.substr(begin, end - begin);

    // Extract count
    begin = end + 2;
    end = text.find(",", begin);
    string count = text.substr(begin, end - begin);

    // Extract data type
    begin = end + 2;
    end = text.find(",", begin);
    string dataType = text.substr(begin, end - begin);

    // Extract root
    begin = end + 2;
    end = text.find(",", begin);
    string root = text.substr(begin, end - begin);

    // Extract communicator id
    begin = end + 2;
    end = text.find(" ", begin);
    string commId = text.substr(begin, end - begin);

    // Create the trace call
    TraceCall tc = createTraceCall("MPI_BCAST", startTime, duration);
    tc.params.push_back(bufferAddr);
    tc.params.push_back(count);
    tc.params.push_back(dataType);
    tc.params.push_back(root);
    tc.params.push_back(commId);
    traceCalls_.push_back(tc);
}

void LanlTraceScanActions::handleMPICommCreate(const string& text)
{
    cerr << __FILE__ << ":" << __LINE__ << ":" << "Handling MPI_COMM_CREATE" << endl;
    cerr << "Bcast Text: " << text << endl;
    assert(0);
}

void LanlTraceScanActions::handleMPICommCompare(const string& text)
{
    cerr << __FILE__ << ":" << __LINE__ << ":" << "Handing MPI_COMM_COMPARE" << endl;
}

void LanlTraceScanActions::handleMPICommDup(const string& text)
{
    //cerr << __FILE__ << ":" << __LINE__ << ":" << "Handing MPI_COMM_DUP" << endl;
    //cerr << "Chunk  is: " << text << endl;

    // Extract start time, duration, comm and newcomm
    string startTime = extractStartTime(text);
    string duration = extractDuration(text);

    size_t pos = text.rfind("\"comm=") + 6;
    string commId = text.substr(pos, 10);
    pos = text.rfind("\"newcomm=") + 9;
    string newCommId = text.substr(pos, 10);

    // Create the trace call
    TraceCall tc = createTraceCall("MPI_COMM_DUP", startTime, duration);
    tc.params.push_back(commId);
    tc.params.push_back(newCommId);
    traceCalls_.push_back(tc);
}

void LanlTraceScanActions::handleMPICommFree(const string& text)
{
    //cerr << __FILE__ << ":" << __LINE__ << ":" << "Handling MPI_COMM_FREE" << endl;
    //cerr << "Chunk  is: " << text << endl;

    // For now, this is a no-op
}

void LanlTraceScanActions::handleMPICommRank(const string& text)
{
    //cerr << __FILE__ << ":" << __LINE__ << ":" << "Handling MPI_COMM_RANK" << endl;
    //cerr << "Chunk  is: " << text << endl;

    // Extract start time, duration, comm and newcomm
    string startTime = extractStartTime(text);
    string duration = extractDuration(text);

    // Don't have enough info in trace, for now this is a no op
}

void LanlTraceScanActions::handleMPICommSize(const string& text)
{
    //cerr << __FILE__ << ":" << __LINE__ << ":" << "Handling MPI_COMM_SIZE" << endl;
    // Don't have enough info in trace, for now this is a no op
}

void LanlTraceScanActions::handleMPICommSplit(const string& text)
{
    cerr << __FILE__ << ":" << __LINE__ << ":" << "Handling MPI_COMM_SPLIT" << endl;
}

void LanlTraceScanActions::handleMPIFileClose(const string& text)
{
    //cerr << __FILE__ << ":" << __LINE__ << ":" << "Handling MPI_FILE_CLOSE" << endl;
    //cerr << "File Close Text: " << text << endl;

    // Extract the start time and duration
    string startTime = text.substr(0, 15);
    size_t pos = text.rfind(" = ");
    string duration = text.substr(pos + 6, 8);

    // Extract the current file handle
    ostringstream oss;
    oss << currentFile_.second;
    string fileHandle = oss.str();

    // Create the trace call
    TraceCall tc = createTraceCall("MPI_FILE_CLOSE", startTime, duration);
    tc.params.push_back(fileHandle);
    traceCalls_.push_back(tc);

    // Clear the current file field
    unsetCurrentFile();
}

void LanlTraceScanActions::handleMPIFileDelete(const string& text)
{
    cerr << __FILE__ << ":" << __LINE__ << ":" << "Unhandled MPI_FILE_DELETE" << endl;
}

void LanlTraceScanActions::handleMPIFileGetAMode(const string& text)
{
    cerr << __FILE__ << ":" << __LINE__ << ":" << "Unhandled MPI_FILE_GET_AMODE" << endl;
}

void LanlTraceScanActions::handleMPIFileGetGroup(const string& text)
{
    cerr << __FILE__ << ":" << __LINE__ << ":" << "Unhandled MPI_FILE_GET_GROUP" << endl;
}

void LanlTraceScanActions::handleMPIFileGetSize(const string& text)
{
    //cerr << __FILE__ << ":" << __LINE__ << ":" << "Unhandled MPI_FILE_GET_SIZE" << endl;
    //cerr << "GetSize: " << text << endl;
    // Extract the start time and duration
    string startTime = text.substr(0, 15);
    size_t pos = text.rfind(" = ");
    string duration = text.substr(pos + 6, 8);

    // Extract the current file handle
    ostringstream oss;
    oss << currentFile_.second;
    string fileHandle = oss.str();

    // Extract the sizeAddr
    size_t begin = text.find("MPI_File_get_size(") + strlen("MPI_File_get_size(");
    size_t end = text.find(",", begin);
    begin = end + 2;
    end = text.find(" ", begin);
    string sizeAddr = text.substr(begin, end - begin);

    // Create the trace call
    TraceCall tc = createTraceCall("MPI_FILE_GET_SIZE", startTime, duration);
    tc.params.push_back(fileHandle);
    tc.params.push_back(sizeAddr);
    traceCalls_.push_back(tc);
}

void LanlTraceScanActions::handleMPIFileOpen(const string& text)
{
    //cerr << __FILE__ << ":" << __LINE__ << ":" << "Unhandled MPI_FILE_OPEN" << endl;
    //cerr << "Open Text: " << text << endl;

    // Extract the start time and duration
    string startTime = text.substr(0, 15);
    size_t pos = text.rfind(" = ");
    string duration = text.substr(pos + 6, 8);

    // Extract the communicator id
    size_t begin = text.find("MPI_File_open(") + strlen("MPI_File_open(");
    size_t end = text.find(",", begin);
    string commId = text.substr(begin, end - begin);

    // Extract the filename
    begin = end + 2;
    end = text.find(",", begin);
    string openName = text.substr(begin, end - begin);

    // Modify the filename to ensure it creates a valid file system path
    string filename = openName;
    if ('/' != filename[0])
    {
        filename = "/autopath/" + openName;
    }

    // Extract the access mode
    begin = end + 2;
    end = text.find(",", begin);
    string accessMode = text.substr(begin, end - begin);

    // Extract the info address
    begin = end + 2;
    end = text.find(",", begin);
    string infoAddr = text.substr(begin, end - begin);

    // Create a file descriptor id
    size_t descriptor = setCurrentFile(filename);
    ostringstream oss;
    oss << descriptor;
    string fileHandle = oss.str();

    // Create the trace call
    TraceCall tc = createTraceCall("MPI_FILE_OPEN", startTime, duration);
    tc.params.push_back(commId);
    tc.params.push_back(filename);
    tc.params.push_back(accessMode);
    tc.params.push_back(infoAddr);
    tc.params.push_back(fileHandle);
    traceCalls_.push_back(tc);
}

void LanlTraceScanActions::handleMPIFileRead(const string& text)
{
    //cerr << __FILE__ << ":" << __LINE__ << ":" << "Unhandled MPI_FILE_READ" << endl;
    //cerr << "Text is: " << text << endl;
    // Extract the start time and duration
    string startTime = extractStartTime(text);
    string duration = extractDuration(text);

    // Set the file handle
    ostringstream oss;
    oss << currentFile_.second;
    string fileHandle = oss.str();

    // Extract the bufferAddr
    size_t begin = text.find("MPI_File_read(") + strlen("MPI_File_read(");
    size_t end = text.find(",", begin);
    begin = end + 2;
    end = text.find(",", begin);
    string bufferAddr = text.substr(begin, end - begin);

    // Extract the count
    begin = end + 2;
    end = text.find(",", begin);
    string count = text.substr(begin, end - begin);

    // Extract the datatype
    begin = end + 2;
    end = text.find(",", begin);
    string datatype = text.substr(begin, end - begin);

    // Extract the statusAddr
    begin = end + 2;
    end = text.find(" ", begin);
    string statusAddr = text.substr(begin, end - begin);

    // Create the trace call
    TraceCall tc = createTraceCall("MPI_FILE_READ", startTime, duration);
    tc.params.push_back(fileHandle);
    tc.params.push_back(bufferAddr);
    tc.params.push_back(count);
    tc.params.push_back(datatype);
    tc.params.push_back(statusAddr);
    traceCalls_.push_back(tc);
}

void LanlTraceScanActions::handleMPIFileReadAll(const string& text)
{
    cerr << __FILE__ << ":" << __LINE__ << ":" << "Unhandled MPI_FILE_READ_ALL" << endl;
}

void LanlTraceScanActions::handleMPIFileReadAt(const string& text)
{
    //cerr << __FILE__ << ":" << __LINE__ << ":" << "Unhandled MPI_FILE_READ_AT" << endl;
    //cerr << "Text is: " << text << endl;

    // Extract the start time and duration
    string startTime = extractStartTime(text);
    string duration = extractDuration(text);

    // Extract the current file handle
    ostringstream oss;
    oss << currentFile_.second;
    string fileHandle = oss.str();

    // Skip over the file handle address
    size_t begin = text.find("MPI_File_read_at(") + strlen("MPI_File_read_at(");
    size_t end = text.find(",", begin);

    // Extract the offset
    begin = end + 2;
    end = text.find(",", begin);
    string offset = text.substr(begin, end - begin);

    // Extract the bufferAddr
    begin = end + 2;
    end = text.find(",", begin);
    string bufferAddr = text.substr(begin, end - begin);

    // Extract the count
    begin = end + 2;
    end = text.find(",", begin);
    string count = text.substr(begin, end - begin);

    // Extract the datatype
    begin = end + 2;
    end = text.find(",", begin);
    string datatype = text.substr(begin, end - begin);

    // Extract the statusAddr
    begin = end + 2;
    end = text.find(" ", begin);
    string statusAddr = text.substr(begin, end - begin);

    // Create the trace call
    TraceCall tc = createTraceCall("MPI_FILE_READ_AT", startTime, duration);
    tc.params.push_back(fileHandle);
    tc.params.push_back(offset);
    tc.params.push_back(bufferAddr);
    tc.params.push_back(count);
    tc.params.push_back(datatype);
    tc.params.push_back(statusAddr);
    traceCalls_.push_back(tc);
}

void LanlTraceScanActions::handleMPIFileReadAtAll(const string& text)
{
    cerr << __FILE__ << ":" << __LINE__ << ":" << "Unhandled MPI_FILE_READ_AT_ALL" << endl;
}

void LanlTraceScanActions::handleMPIFileSeek(const string& text)
{
    //cerr << __FILE__ << ":" << __LINE__ << ":" << "Unhandled MPI_FILE_SEEK" << endl;
    //cerr << "Seek text is: " << text << endl;
    // Extract the start time and duration
    string startTime = extractStartTime(text);
    string duration = extractDuration(text);

    // Extract the current file handle
    ostringstream oss;
    oss << currentFile_.second;
    string fileHandle = oss.str();

    // Skip over the file handle address
    size_t begin = text.find("MPI_File_seek(") + strlen("MPI_File_seek(");
    size_t end = text.find(",", begin);

    // Extract the offset
    begin = end + 2;
    end = text.find(",", begin);
    string offset = text.substr(begin, end - begin);

    // Extract the whence data
    begin = end + 2;
    end = text.find_first_of(" )", begin);
    string whence = text.substr(begin, end - begin);

    // Create the trace call
    TraceCall tc = createTraceCall("MPI_FILE_SEEK", startTime, duration);
    tc.params.push_back(fileHandle);
    tc.params.push_back(offset);
    tc.params.push_back(whence);
    traceCalls_.push_back(tc);
}


void LanlTraceScanActions::handleMPIFileSetSize(const string& text)
{
    //cerr << __FILE__ << ":" << __LINE__ << ":" << "Unhandled MPI_FILE_SET_SIZE" << endl;
    //cerr << "Set Size Text: " << text << endl;

    // Extract the start time and duration
    string startTime = extractStartTime(text);
    string duration = extractDuration(text);

    // Extract the current file handle
    ostringstream oss;
    oss << currentFile_.second;
    string fileHandle = oss.str();

    // Skip over the file handle address
    size_t begin = text.find("MPI_File_set_size(") + strlen("MPI_File_set_size(");
    size_t end = text.find(",", begin);

    // Extract the size
    begin = end + 2;
    end = text.find(" ", begin);
    string size= text.substr(begin, end - begin);

    // Create the trace call
    TraceCall tc = createTraceCall("MPI_FILE_SET_SIZE", startTime, duration);
    tc.params.push_back(fileHandle);
    tc.params.push_back(size);
    traceCalls_.push_back(tc);
}


void LanlTraceScanActions::handleMPIFileWrite(const string& text)
{
    //cerr << __FILE__ << ":" << __LINE__ << ":" << "Unhandled MPI_FILE_WRITE" << endl;
    //cerr << "Text is: " << text << endl;
    // Extract the start time and duration
    string startTime = extractStartTime(text);
    string duration = extractDuration(text);

    // Extract the current file handle
    ostringstream oss;
    oss << currentFile_.second;
    string fileHandle = oss.str();

    // Skip over the file handle address
    size_t begin = text.find("MPI_File_write(") + strlen("MPI_File_write(");
    size_t end = text.find(",", begin);

    // Extract the bufferAddr
    begin = end + 2;
    end = text.find(",", begin);
    string bufferAddr = text.substr(begin, end - begin);

    // Extract the count
    begin = end + 2;
    end = text.find(",", begin);
    string count = text.substr(begin, end - begin);

    // Extract the datatype
    begin = end + 2;
    end = text.find(",", begin);
    string datatype = text.substr(begin, end - begin);

    // Extract the statusAddr
    begin = end + 2;
    end = text.find(" ", begin);
    string statusAddr = text.substr(begin, end - begin);

    // Create the trace call
    TraceCall tc = createTraceCall("MPI_FILE_WRITE", startTime, duration);
    tc.params.push_back(fileHandle);
    tc.params.push_back(bufferAddr);
    tc.params.push_back(count);
    tc.params.push_back(datatype);
    tc.params.push_back(statusAddr);
    traceCalls_.push_back(tc);
}

void LanlTraceScanActions::handleMPIFileWriteAll(const string& text)
{
    cerr << __FILE__ << ":" << __LINE__ << ":" << "Unhandled MPI_FILE_WRITE_ALL" << endl;
}

void LanlTraceScanActions::handleMPIFileWriteAt(const string& text)
{
    //cerr << __FILE__ << ":" << __LINE__ << ":" << "Handing MPI_FILE_WRITE_AT" << endl;
    //cerr << "Text is: " << text << endl;

    // Extract the start time and duration
    string startTime = extractStartTime(text);
    string duration = extractDuration(text);

    // Extract the current file handle
    ostringstream oss;
    oss << currentFile_.second;
    string fileHandle = oss.str();

    // Skip over the file handle address
    size_t begin = text.find("MPI_File_write_at(") + strlen("MPI_File_write_at(");
    size_t end = text.find(",", begin);

    // Extract the offset
    begin = end + 2;
    end = text.find(",", begin);
    string offset = text.substr(begin, end - begin);

    // Extract the bufferAddr
    begin = end + 2;
    end = text.find(",", begin);
    string bufferAddr = text.substr(begin, end - begin);

    // Extract the count
    begin = end + 2;
    end = text.find(",", begin);
    string count = text.substr(begin, end - begin);

    // Extract the datatype
    begin = end + 2;
    end = text.find(",", begin);
    string datatype = text.substr(begin, end - begin);

    // Extract the statusAddr
    begin = end + 2;
    end = text.find(" ", begin);
    string statusAddr = text.substr(begin, end - begin);

    // Create the trace call
    TraceCall tc = createTraceCall("MPI_FILE_WRITE_AT", startTime, duration);
    tc.params.push_back(fileHandle);
    tc.params.push_back(offset);
    tc.params.push_back(bufferAddr);
    tc.params.push_back(count);
    tc.params.push_back(datatype);
    tc.params.push_back(statusAddr);
    traceCalls_.push_back(tc);
}

void LanlTraceScanActions::handleMPIFileWriteAtAll(const string& text)
{
    cerr << __FILE__ << ":" << __LINE__ << ":" << "Unhandled MPI_FILE_WRITE_AT_ALL" << endl;
}

void LanlTraceScanActions::handleMPIFinalize(const string& text)
{
    //cerr << __FILE__ << ":" << __LINE__ << ":" << "Handing MPI_FINALIZE" << endl;
    //cerr << "Text is: " << text << endl;

    // Extract the start time and duration
    string startTime = extractStartTime(text);
    string duration = extractDuration(text);

    // Create the trace call
    TraceCall tc = createTraceCall(CPU_PHASE_CALLNAME, startTime, duration);
    traceCalls_.push_back(tc);
}

void LanlTraceScanActions::handleMPIGetElements(const string& text)
{
    //cerr << __FILE__ << ":" << __LINE__ << ":"
    //     << "Handling MPI_GET_ELEMENTS" << endl;

    // TODO: For now, this is a no-op
}

void LanlTraceScanActions::handleMPIGetProcessorName(const string& text)
{
    //cerr << __FILE__ << ":" << __LINE__ << ":"
    //     << "Handling MPI_GET_PROCESSOR_NAME" << endl;

    // TODO: For now, this is a no-op
}

void LanlTraceScanActions::handleMPIInfoCreate(const string& text)
{
    //cerr << __FILE__ << ":" << __LINE__ << ":"
    //     << "Handling MPI_INFO_CREATE" << endl;

    // For now, this is a no-op
}

void LanlTraceScanActions::handleMPIInfoDup(const string& text)
{
    //cerr << __FILE__ << ":" << __LINE__ << ":"
    //     << "Handling MPI_INFO_FREE" << endl;

    // For now, this is a no-op
}

void LanlTraceScanActions::handleMPIInfoFree(const string& text)
{
    //cerr << __FILE__ << ":" << __LINE__ << ":"
    //     << "Handling MPI_INFO_FREE" << endl;

    // For now, this is a no-op
}

void LanlTraceScanActions::handleMPIInit(const string& text)
{
    //cerr << __FILE__ << ":" << __LINE__ << ":"
    //     << "ERROR: Unhandled MPI_INIT" << endl;
    //cerr << "Init Text: " << text << endl;
    // Extract the start time and duration
    string startTime = extractStartTime(text);
    string duration = extractDuration(text);

    // TODO: For now, this is a no-op
    TraceCall tc = createTraceCall(CPU_PHASE_CALLNAME, startTime, duration);
    traceCalls_.push_back(tc);
}

void LanlTraceScanActions::handleMPITypeSize(const string& text)
{
    //cerr << __FILE__ << ":" << __LINE__ << ":"
    //     << "Handling MPI_TYPE_SIZE" << endl;

    // TODO: For now, this is a no-op
}

void LanlTraceScanActions::handleMPIWTime(const string& text)
{
    //cerr << __FILE__ << ":" << __LINE__ << ":"
    //     << "Handling MPI_WTIME" << endl;

    // TODO: For now, this is a no-op
}

size_t LanlTraceScanActions::setCurrentFile(const string& filename)
{
    if (currentFile_.first != "" &&
        currentFile_.second != 0)
    {
        cerr << "ERROR: Trace requires more than a single file open at one time."
             << endl;
    }
    size_t fd = nextFileDescriptor_++;
    currentFile_ = make_pair(filename, fd);
    allFilenames_.insert(filename);
    return fd;
}

void LanlTraceScanActions::unsetCurrentFile()
{
    currentFile_ = make_pair("", 0);
}

string LanlTraceScanActions::extractStartTime(const string& text)
{
    return text.substr(0, 15);
}

string LanlTraceScanActions::extractDuration(const string& text)
{
    size_t pos = text.rfind(" = ");
    return text.substr(pos + 6, 8);
}

LanlTraceScanActions::TraceCall LanlTraceScanActions::createTraceCall(const string& callName,
                                                                      const string& timeStamp,
                                                                      const string& durationString)
{
    // Convert the invoke time into a double
    timespec startTime = createTime(timeStamp);
    timespec exeTime = diff_timespec(epochTime_, startTime);
    double invokeTime = exeTime.tv_sec + (exeTime.tv_nsec * SECONDS_PER_NANOSECOND);

    // Convert the duration into a double
    double duration;
    istringstream iss(durationString);
    iss >> duration;

    TraceCall tc;
    tc.id = currentTraceCallId_++;
    tc.callname = callName;
    tc.invokeTime = invokeTime;
    tc.duration = duration;
    tc.returnValue = "0";
    return tc;
}

LanlTraceScanActions::TraceCall LanlTraceScanActions::createCpuPhase(double beginTime,
                                                                     double duration)
{
    // Create the CPU Phase
    TraceCall tc;
    tc.id = currentTraceCallId_++;
    tc.callname = "CPU_PHASE";
    tc.invokeTime = beginTime;
    tc.duration = duration;
    tc.returnValue = "0";
    return tc;
}

timespec LanlTraceScanActions::createTime(const string& timeStamp)
{
    // Create the timeinfo (wday and yday are ignored initially)
    struct tm timeinfo;
    memset(&timeinfo, 0, sizeof(timeinfo));
    timeinfo.tm_mday = 1;
    timeinfo.tm_mon = 0;
    timeinfo.tm_year = 2008;

    // Times are of the format dd:hh:ss.ssssss
    int hours, minutes;
    double seconds;
    sscanf(timeStamp.c_str(), "%d:%d:%lf", &hours, &minutes, &seconds);
    timeinfo.tm_sec = seconds;
    timeinfo.tm_min = minutes;
    timeinfo.tm_hour = hours;

    // Convert the fractional seconds into nanoseconds
    double partialSeconds = seconds - long(seconds);
    long nanoseconds = partialSeconds * NANOSECONDS_PER_SECOND;

    // Create the timespec
    timespec time;
    memset(&time, 0, sizeof(time));
    time.tv_sec = mktime(&timeinfo);
    time.tv_nsec = nanoseconds;
    return time;
}

std::ostream& operator<<(std::ostream& ost,
                         const LanlTraceScanActions::TraceCall& tc)
{
    ost << tc.id
        << " " << tc.callname
        << " " << setiosflags(ios::fixed) << setprecision(6) << tc.invokeTime
        << " " << setiosflags(ios::fixed) << setprecision(6) << tc.duration
        << " " << tc.returnValue;
    for (size_t i = 0; i < tc.params.size(); i++)
    {
        ost << " " << tc.params[i];
    }
    return ost;
}

/*
 * Local variables:
 *  indent-tabs-mode: nil
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 *
 * vim: ts=4 sts=4 sw=4 expandtab
 */
