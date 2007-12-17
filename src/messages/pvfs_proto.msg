//
// This file is part of Hecios
//
// Copyright (C) 2007 Walt Ligon
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

//
// Parallel File System request proto message definitions
//

cplusplus {{
#include "basic_types.h"
#include "pfs_types.h"
#include "file_distribution.h"
typedef FileDistribution* FileDistributionPtr;
}}

// Forward declarations
class cFSM;
class noncobject FSDataType;
class noncobject FSHandle;
class noncobject FSMetaData;
class noncobject FSOffset;
class noncobject FSSize;
class noncobject FileDistributionPtr;

// PVFS Protocol Message Kind values
enum spfsMessageKind 
{
    SPFS_CREATE_REQUEST = 401;
    SPFS_CREATE_RESPONSE = 402;
    SPFS_REMOVE_REQUEST = 403;
    SPFS_REMOVE_RESPONSE = 404;
    SPFS_READ_REQUEST = 405;
    SPFS_READ_RESPONSE = 406;
    SPFS_WRITE_REQUEST = 407;
    SPFS_WRITE_RESPONSE = 408;
    SPFS_WRITE_COMPLETION_RESPONSE = 409;
    SPFS_GET_ATTR_REQUEST = 410;
    SPFS_GET_ATTR_RESPONSE = 411;
    SPFS_SET_ATTR_REQUEST = 412;
    SPFS_SET_ATTR_RESPONSE = 413;
    SPFS_LOOKUP_PATH_REQUEST = 414;
    SPFS_LOOKUP_PATH_RESPONSE = 415;
    SPFS_CREATE_DIR_ENT_REQUEST = 416;
    SPFS_CREATE_DIR_ENT_RESPONSE = 517;
    SPFS_REMOVE_DIR_ENT_REQUEST = 418;
    SPFS_REMOVE_DIR_ENT_RESPONSE = 419;
    SPFS_CHANGE_DIR_ENT_REQUEST = 420;
    SPFS_CHANGE_DIR_ENT_RESPONSE = 421;
    SPFS_TRUNCATE_REQUEST = 422;
    SPFS_TRUNCATE_RESPONSE = 423;
    SPFS_MAKE_DIR_REQUEST = 424;
    SPFS_MAKE_DIR_RESPONSE = 425;
    SPFS_READ_DIR_REQUEST = 426;
    SPFS_READ_DIR_RESPONSE = 427;
    SPFS_FLUSH_REQUEST = 428;
    SPFS_FLUSH_RESPONSE = 429;
    SPFS_STAT_REQUEST = 430;
    SPFS_STAT_RESPONSE = 431;
    SPFS_LIST_ATTR_REQUEST = 432;
    SPFS_LIST_ATTR_RESPONSE = 433;
    SPFS_DATA_FLOW_START = 434;
    SPFS_DATA_FLOW_FINISH = 435;
};

enum spfsLookupStatus {
    SPFS_FOUND = 1;
    SPFS_PARTIAL = 2;
    SPFS_NOTFOUND = 3;
}


// File request abstract base class
message spfsRequest
{
    fields:
        int fsId;
        FSHandle handle;   // indicates server address and object

        // internal fields
        cFSM state;
        int bmiConnectionId;
};

// File system response abstract base class
message spfsResponse
{
    fields:
        bool requestSucceeded;
};

// Create a file system object
message spfsCreateRequest extends spfsRequest
{
    fields:
        int objectType;
};

// Create a file system object
message spfsCreateResponse extends spfsResponse
{
    fields:
        FSHandle handle;
};

// Remove a file system object
message spfsRemoveRequest extends spfsRequest
{
    fields:
};

// Remove a file system object
message spfsRemoveResponse extends spfsResponse
{
};

// Read object data
message spfsReadRequest extends spfsRequest
{
    fields:
        int serverNo;
        int serverCnt;
        FSOffset offset;
        FSDataType dataType;
        unsigned long count;
        FileDistributionPtr dist;
        int flowTag;
};

// Read file data
message spfsReadResponse extends spfsResponse
{
    fields:
};

// Write object data
message spfsWriteRequest extends spfsRequest
{
    fields:
        int serverNo;
        int serverCnt;
        unsigned long offset;
        int count;
        FSDataType dataType;
        FileDistributionPtr dist;
        int flowTag;
};

// Write file data
message spfsWriteResponse extends spfsResponse
{
    fields:
};

// Write completion
message spfsWriteCompletionResponse extends spfsResponse
{
    fields:
        FSSize bytesWritten;
};

// Get file attributes
message spfsGetAttrRequest extends spfsRequest
{
    fields:
        string path; // remove this ???
};

// Get attributes
message spfsGetAttrResponse extends spfsResponse
{
    fields:
        FSMetaData meta;
};

// Set file attributes
message spfsSetAttrRequest extends spfsRequest
{
    fields:
        FSMetaData meta;
};

// Set attributes
message spfsSetAttrResponse extends spfsResponse
{
    fields:
        string attr[]; // remove this ???
};

// Resolve a filesystem path
message spfsLookupPathRequest extends spfsRequest
{
    fields:
        string path;
};

// Lookup a path
message spfsLookupPathResponse extends spfsResponse
{
    fields:
        int status enum(spfsLookupStatus);
        FSHandle handles[];
        unsigned long handleCount;
        unsigned long attrCount;
};

// Create a directory entry
message spfsCreateDirEntRequest extends spfsRequest
{
    fields:
        FSHandle parentHandle;
        FSHandle newHandle;
        string entry;
};

// Create a directory entry
message spfsCreateDirEntResponse extends spfsResponse
{
};

// Remove a directory entry
message spfsRemoveDirEntRequest extends spfsRequest
{
    fields:
        string entry;
};

// Remove a directory entry
message spfsRemoveDirEntResponse extends spfsResponse
{
    fields:
        FSHandle removedHandle;
};

// Resolve a filesystem path
message spfsChangeDirEntRequest extends spfsRequest
{
    fields:
        FSHandle parentHandle;
        FSHandle newHandle;
        string entry;
};

// Change a directory entry
message spfsChangeDirEntResponse extends spfsResponse
{
    fields:
        FSHandle oldDirEntHandle;
};

// Resolve a filesystem path
message spfsTruncateRequest extends spfsRequest
{
    fields:
        FSSize newSize;
        int flags;
};

// Truncate a data object
message spfsTruncateResponse extends spfsResponse
{
};

// Resolve a filesystem path
message spfsMakeDirRequest extends spfsRequest
{
    fields:
        FSSize handleExtents[];
};

// Create a directory
message spfsMakeDirResponse extends spfsResponse
{
    fields:
};

// Resolve a filesystem path
message spfsReadDirRequest extends spfsRequest
{
    fields:
        FSOffset dirOffset;
        unsigned long dirEntCount;
};

// Read a directory
message spfsReadDirResponse extends spfsResponse
{
    fields:
        FSOffset dirOffset;
        string entries[];
        unsigned long entryCount;
        unsigned long directoryVersion;
};

// Resolve a filesystem path
message spfsFlushRequest extends spfsRequest
{
    fields:
        string path;
};

// Flush a data object
message spfsFlushResponse extends spfsResponse
{
};

// Resolve a parallel filesystem path
message spfsStatRequest extends spfsRequest
{
    fields:
        int flags;
};

// Stat parallel filesystem
message spfsStatResponse extends spfsResponse
{
    fields:
        string stat;
};

// Resolve a parallel filesystem path
message spfsListAttrRequest extends spfsRequest
{
    fields:
        FSHandle handles[];
        unsigned long handleCount;
        int attrMask;
};

// List data object attributes
message spfsListAttrResponse extends spfsResponse
{
    fields:
        unsigned long numHandles;
};

// Begin the data flow
message spfsDataFlowStart extends spfsRequest
{
    fields:
        // Flow information
        int flowType;
        int flowMode;
        int flowId; // Set by the job manager

        // The bmi connection and tag to use for this flow
        int bmiConnectionId;
        int bmiTag;

        // Data information
        FSOffset offset;
        FSDataType dataType;
        unsigned long count;
        FileDistributionPtr dist;
};

// Indicate the data flow has finished
message spfsDataFlowFinish extends spfsResponse
{
    fields:
        int flowId;
        FSSize flowSize;
};

//
// Local variables:
//  indent-tabs-mode: nil
//  c-indent-level: 4
//  c-basic-offset: 4
// End:
//
// vim: ts=4 sts=4 sw=4 expandtab
//
