#ifndef UMD_IO_TRACE_H
#define UMD_IO_TRACE_H
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

#include <fstream>
#include <string>
#include "io_trace.h"

/**
 * An I/O trace in the University of Maryland format.  Traces are available
 * at the following url: http://www.cs.umd.edu/xxx
 */
class UMDIOTrace: public IOTrace
{
public:

    /** Constructor */
    UMDIOTrace(int numProcs, std::string traceFileName);

    /** Destructor */
    virtual ~UMDIOTrace();

    /** @return true if the trace file is valid */
    bool isValid() const {return traceFile_;};

    /** @return true if more records remain */
    bool hasMoreRecords() const { return (curRecord_ < numRecords_); };
    
    /** @return the next IOTraceRecord */
    virtual IOTraceRecord* nextRecord();

private:

    enum OpType {OPEN = 0, CLOSE = 1, READ = 2,
                 WRITE = 3, SEEK = 4, LISTIO_HEADER = 5};

    /** @return the next IOTraceRecord */
    IOTraceRecord* createIOTraceRecord(OpType opType, int fileId,
                                  long offset, long length);
    
    std::string traceFileName_;
    mutable std::ifstream traceFile_;

    // Header data
    int numProcs_;
    int numFiles_;
    int numRecords_;
    int offsetToTraceRecords_;
    
    mutable int curRecord_;
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
