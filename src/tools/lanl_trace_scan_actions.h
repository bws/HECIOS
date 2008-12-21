#ifndef LANL_TRACE_SCAN_ACTIONS_H_
#define LANL_TRACE_SCAN_ACTIONS_H_
//
// This file is part of Hecios
//
// Copyright (C) 2008 bradles
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
#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */
    /** C Wrapper to perform trace parsing for MPI_ALLREDUCE */
    void handleMPIAllReduce(const char* text);

    /** C Wrapper to perform trace parsing for MPI_BARRIER */
    void handleMPIBarrier(const char* text);

    /** C Wrapper to perform trace parsing for MPI_BCAST */
    void handleMPIBcast(const char* text);

    /** C Wrapper to perform trace parsing for MPI_COMM_CREATE */
    void handleMPICommCreate(const char* text);

    /** C Wrapper to perform trace parsing for MPI_COMM_COMPARE */
    void handleMPICommCompare(const char* text);

    /** C Wrapper to perform trace parsing for MPI_COMM_DUP */
    void handleMPICommDup(const char* text);

    /** C Wrapper to perform trace parsing for MPI_COMM_Free */
    void handleMPICommFree(const char* text);

    /** C Wrapper to perform trace parsing for MPI_COMM_RANK */
    void handleMPICommRank(const char* text);

    /** C Wrapper to perform trace parsing for MPI_COMM_SIZE */
    void handleMPICommSize(const char* text);

    /** C Wrapper to perform trace parsing for MPI_COMM_SPLIT */
    void handleMPICommSplit(const char* text);

    /** C Wrapper to perform trace parsing for MPI_FILE_CLOSE */
    void handleMPIFileClose(const char* text);

    /** C Wrapper to perform trace parsing for MPI_FILE_DELETE */
    void handleMPIFileDelete(const char* text);

    /** C Wrapper to perform trace parsing for MPI_FILE_GET_AMODE */
    void handleMPIFileGetAMode(const char* text);

    /** C Wrapper to perform trace parsing for MPI_FILE_GET_GROUP */
    void handleMPIFileGetGroup(const char* text);

    /** C Wrapper to perform trace parsing for MPI_FILE_GET_SIZE */
    void handleMPIFileGetSize(const char* text);

    /** C Wrapper to perform trace parsing for MPI_FILE_OPEN */
    void handleMPIFileOpen(const char* text);

    /** C Wrapper to perform trace parsing for MPI_FILE_READ */
    void handleMPIFileRead(const char* text);

    /** C Wrapper to perform trace parsing for MPI_FILE_READ_ALL */
    void handleMPIFileReadAll(const char* text);

    /** C Wrapper to perform trace parsing for MPI_FILE_READ_AT */
    void handleMPIFileReadAt(const char* text);

    /** C Wrapper to perform trace parsing for MPI_FILE_READ_AT_ALL */
    void handleMPIFileReadAtAll(const char* text);

    /** C Wrapper to perform trace parsing for MPI_FILE_SEEK */
    void handleMPIFileSeek(const char* text);

    /** C Wrapper to perform trace parsing for MPI_FILE_SET_SIZE */
    void handleMPIFileSetSize(const char* text);

    /** C Wrapper to perform trace parsing for MPI_FILE_WRITE */
    void handleMPIFileWrite(const char* text);

    /** C Wrapper to perform trace parsing for MPI_FILE_WRITE_ALL */
    void handleMPIFileWriteAll(const char* text);

    /** C Wrapper to perform trace parsing for MPI_FILE_WRITE_AT */
    void handleMPIFileWriteAt(const char* text);

    /** C Wrapper to perform trace parsing for MPI_FILE_WRITE_AT_ALL */
    void handleMPIFileWriteAtAll(const char* text);

    /** C Wrapper to perform trace parsing for MPI_FINALIZE */
    void handleMPIFinalize(const char* text);

    /** C Wrapper to perform trace parsing for MPI_GET_ELEMENTS */
    void handleMPIGetElements(const char* text);

    /** C Wrapper to perform trace parsing for MPI_GET_PROCESSOR_NAME */
    void handleMPIGetProcessorName(const char* text);

    /** C Wrapper to perform trace parsing for MPI_INFO_CREATE */
    void handleMPIInfoCreate(const char* text);

    /** C Wrapper to perform trace parsing for MPI_INFO_DUP */
    void handleMPIInfoDup(const char* text);

    /** C Wrapper to perform trace parsing for MPI_INFO_FREE */
    void handleMPIInfoFree(const char* text);

    /** C Wrapper to perform trace parsing for MPI_INIT */
    void handleMPIInit(const char* text);

    /** C Wrapper to perform trace parsing for MPI_TYPE_SIZE */
    void handleMPITypeSize(const char* text);

    /** C Wrapper to perform trace parsing for MPI_WTIME */
    void handleMPIWTime(const char* text);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#ifdef __cplusplus
#include <cstddef>
#include <time.h>
#include <set>
#include <string>
#include <vector>
#include "singleton.h"

/** Class for parsing LANL trace scan units */
class LanlTraceScanActions  : public Singleton<LanlTraceScanActions>
{
public:
    /** Enable singleton construction */
    friend class Singleton<LanlTraceScanActions>;

    /** Constant useful for converting seconds to nanoseconds */
    static const double NANOSECONDS_PER_SECOND = 1000000000.0;

    /** Constant useful for converting seconds to nanoseconds */
    static const double SECONDS_PER_NANOSECOND = 1e-9;

    /** Difference between double before difference is considered significant */
    static const double EPSILON = 1e-6;

    /** Structure for describing a trace call */
    struct TraceCall
    {
        /** unique id for this call */
        std::size_t id;

        /** the time this call began */
        double invokeTime;

        /** the time this call lasted */
        double duration;

        /** the name of the call */
        std::string callname;

        /** the name of the call */
        std::string returnValue;

        /** the call parameters */
        std::vector<std::string> params;
    };

    /** Destructor */
    ~LanlTraceScanActions();

    /** @return all of the filenames opened by the trace calls */
    std::set<std::string> getFilenames() const { return allFilenames_; };

    /** Emit the file system configuration file */
    void emitOutputConfigFile(const std::string& configFilename);

    /** Emit the trace output */
    void emitTraceCalls(std::ostream& ost);

    /** Set the origin time for the trace */
    void setEpochTime(const std::string& timestamp);

    /** Perform trace parsing for MPI_ALLREDUCE */
    void handleMPIAllReduce(const std::string& text);

    /** Perform trace parsing for MPI_BARRIER */
    void handleMPIBarrier(const std::string& text);

    /** Perform trace parsing for MPI_BCAST */
    void handleMPIBcast(const std::string& text);

    /** Perform trace parsing for MPI_COMM_CREATE */
    void handleMPICommCreate(const std::string& text);

    /** Perform trace parsing for MPI_COMM_COMPARE */
    void handleMPICommCompare(const std::string& text);

    /** Perform trace parsing for MPI_COMM_DUP */
    void handleMPICommDup(const std::string& text);

    /** Perform trace parsing for MPI_COMM_FREE */
    void handleMPICommFree(const std::string& text);

    /** Perform trace parsing for MPI_COMM_RANK */
    void handleMPICommRank(const std::string& text);

    /** Perform trace parsing for MPI_COMM_SIZE */
    void handleMPICommSize(const std::string& text);

    /** Perform trace parsing for MPI_COMM_SPLIT */
    void handleMPICommSplit(const std::string& text);

    /** Perform trace parsing for MPI_FILE_CLOSE */
    void handleMPIFileClose(const std::string& text);

    /** Perform trace parsing for MPI_FILE_DELETE */
    void handleMPIFileDelete(const std::string& text);

    /** Perform trace parsing for MPI_FILE_GET_AMODE */
    void handleMPIFileGetAMode(const std::string& text);

    /** Perform trace parsing for MPI_FILE_GET_GROUP */
    void handleMPIFileGetGroup(const std::string& text);

    /** Perform trace parsing for MPI_FILE_GET_SIZE */
    void handleMPIFileGetSize(const std::string& text);

    /** Perform trace parsing for MPI_FILE_OPEN */
    void handleMPIFileOpen(const std::string& text);

    /** Perform trace parsing for MPI_FILE_SEEK */
    void handleMPIFileSeek(const std::string& text);

    /** Perform trace parsing for MPI_FILE_SET_SIZE */
    void handleMPIFileSetSize(const std::string& text);

    /** Perform trace parsing for MPI_FILE_READ */
    void handleMPIFileRead(const std::string& text);

    /** Perform trace parsing for MPI_FILE_READ_ALL */
    void handleMPIFileReadAll(const std::string& text);

    /** Perform trace parsing for MPI_FILE_READ_AT */
    void handleMPIFileReadAt(const std::string& text);

    /** Perform trace parsing for MPI_FILE_READ_AT_ALL */
    void handleMPIFileReadAtAll(const std::string& text);

    /** Perform trace parsing for MPI_FILE_WRITE */
    void handleMPIFileWrite(const std::string& text);

    /** Perform trace parsing for MPI_FILE_WRITE_ALL */
    void handleMPIFileWriteAll(const std::string& text);

    /** Perform trace parsing for MPI_FILE_WRITE_AT */
    void handleMPIFileWriteAt(const std::string& text);

    /** Perform trace parsing for MPI_FILE_WRITE_AT_ALL */
    void handleMPIFileWriteAtAll(const std::string& text);

    /** Perform trace parsing for MPI_FINALIZE */
     void handleMPIFinalize(const std::string& text);

     /** Perform trace parsing for MPI_GET_ELEMENTS */
    void handleMPIGetElements(const std::string& text);

    /** Perform trace parsing for MPI_GET_PROCESSOR_NAME */
    void handleMPIGetProcessorName(const std::string& text);

    /** Perform trace parsing for MPI_INFO_CREATE */
    void handleMPIInfoCreate(const std::string& text);

    /** Perform trace parsing for MPI_INFO_DUP */
    void handleMPIInfoDup(const std::string& text);

    /** Perform trace parsing for MPI_INFO_FREE */
    void handleMPIInfoFree(const std::string& text);

    /** Perform trace parsing for MPI_INIT */
    void handleMPIInit(const std::string& text);

    /** Perform trace parsing for MPI_TYPE_SIZE */
    void handleMPITypeSize(const std::string& text);

    /** Perform trace parsing for MPI_WTIME */
    void handleMPIWTime(const std::string& text);

private:
    LanlTraceScanActions();

    /** @return substring [0,15] as the start time */
    std::string extractStartTime(const std::string& text);

    /** @return the closing time duration */
    std::string extractDuration(const std::string& text);

    /** @return the descriptor for the filename */
    std::size_t setCurrentFile(const std::string& filename);

    /** Clear the current file */
    void unsetCurrentFile();

    /** @return a TraceCall with the name and id set */
    TraceCall createTraceCall(const std::string& callName,
                              const std::string& timestamp,
                              const std::string& duration);

    /** @return a CPU_PHASE Tracecall with specified begin time and duration */
    TraceCall createCpuPhase(double beginTime, double duration);

    /** @return time_t representing the time stamp */
    static timespec createTime(const std::string& timeStamp);

    static std::size_t currentTraceCallId_;

    std::vector<TraceCall> traceCalls_;

    timespec epochTime_;

    std::pair<std::string, std::size_t> currentFile_;

    std::size_t nextFileDescriptor_;

    /** All the filenames opened during the scan */
    std::set<std::string> allFilenames_;
};

std::ostream& operator<<(std::ostream& ost, const LanlTraceScanActions::TraceCall& tc);

#endif /* __cplusplus */

#endif /* LANL_TRACE_SCAN_ACTIONS_H_ */

/*
 * Local variables:
 *  indent-tabs-mode: nil
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 *
 * vim: ts=4 sts=4 sw=4 expandtab
 */
