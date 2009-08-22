//
// This file is part of Hecios
//
// Copyright (C) 2007 Walter Ligon, III
// Copyright (C) 2007,2008,2009 Brad Settlemyer
//
// This file is distributed WITHOUT ANY WARRANTY. See the file 'License.txt'
// for details on this and other legal matters.
//

//
// Parallel File System request proto message definitions
//

cplusplus {{
#include "basic_types.h"
#include "data_type.h"
#include "file_distribution.h"
#include "file_view.h"
#include "pfs_types.h"
typedef FileDistribution* FileDistributionPtr;
typedef FileView* FileViewPtr;
typedef DataType* DataTypePtr;
typedef void* VoidPtr;
}}

// Forward declarations
class cFSM;
class noncobject ConnectionId;
class noncobject DataTypePtr;
class noncobject FileDistributionPtr;
class noncobject FileViewPtr;
class noncobject FSHandle;
class noncobject FSLookupStatus;
class noncobject FSMetaData;
class noncobject FSObjectType;
class noncobject FSOffset;
class noncobject FSSize;
class noncobject VoidPtr;

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
    SPFS_COLLECTIVE_CREATE_REQUEST = 436;
    SPFS_COLLECTIVE_CREATE_RESPONSE = 437;
    SPFS_COLLECTIVE_GET_ATTR_REQUEST = 438;
    SPFS_COLLECTIVE_GET_ATTR_RESPONSE = 439;
    SPFS_COLLECTIVE_REMOVE_REQUEST = 440;
    SPFS_COLLECTIVE_REMOVE_RESPONSE = 441;
    SPFS_INVALIDATE_PAGES_REQUEST = 442;
    SPFS_INVALIDATE_PAGES_WRAPPER_REQUEST = 443;
    SPFS_READ_PAGES_REQUEST = 444;
    SPFS_READ_PAGES_RESPONSE = 445;
};

// File request abstract base class
packet spfsRequest
{
    fields:
        int fsId;
        FSHandle handle;   // indicates server address and object
        bool autoCleanup = true;

        // internal fields
        cFSM state;
        ConnectionId bmiConnectionId;
};

// File system response abstract base class
packet spfsResponse
{
    fields:
        bool requestSucceeded;
        long byteLength;
};

// Create a file system object
packet spfsCreateRequest extends spfsRequest
{
    fields:
        FSObjectType objectType;
};

// Create a file system object
packet spfsCreateResponse extends spfsResponse
{
    fields:
        FSHandle handle;
};

// Remove a file system object
packet spfsRemoveRequest extends spfsRequest
{
    fields:
        FSObjectType objectType;
};

// Remove a file system object
packet spfsRemoveResponse extends spfsResponse
{
};

// Read object data
packet spfsReadRequest extends spfsRequest
{
    fields:
    	FSHandle metaHandle;
        FSOffset offset;
        FSSize dataSize;
        FileViewPtr view;
        FileDistributionPtr dist;
    	FSSize bstreamSize;
    	FSSize localSize;
        int clientFlowBmiTag;
        int serverFlowBmiTag;
};

// Read file data
packet spfsReadResponse extends spfsResponse
{
    fields:
};

// Write object data
packet spfsWriteRequest extends spfsRequest
{
    fields:
    	FSHandle metaHandle;
        FSOffset offset;
        FSSize dataSize;
        FileViewPtr view;
        FileDistributionPtr dist;
        int clientFlowBmiTag;
        int serverFlowBmiTag;
};

// Write file data
packet spfsWriteResponse extends spfsResponse
{
    fields:
};

// Write completion
packet spfsWriteCompletionResponse extends spfsResponse
{
    fields:
        FSSize bytesWritten;
};

// Get file attributes
packet spfsGetAttrRequest extends spfsRequest
{
    fields:
        FSObjectType objectType;
        string path; // remove this ???
};

// Get attributes
packet spfsGetAttrResponse extends spfsResponse
{
    fields:
        FSMetaData meta;
};

// Set file attributes
packet spfsSetAttrRequest extends spfsRequest
{
    fields:
        FSObjectType objectType;
        FSMetaData meta;
};

// Set attributes
packet spfsSetAttrResponse extends spfsResponse
{
    fields:
        string attr[]; // remove this ???
};

// Resolve a filesystem path
packet spfsLookupPathRequest extends spfsRequest
{
    fields:
        string filename;
        int numResolvedSegments;
        bool isCreate;
        int locallyResolvedSegments = 0;
};

// Lookup a path
packet spfsLookupPathResponse extends spfsResponse
{
    fields:
        FSLookupStatus status;
        int numResolvedSegments;
};

// Create a directory entry
packet spfsCreateDirEntRequest extends spfsRequest
{
    fields:
        FSHandle newEntryHandle;
        string entry;
};

// Create a directory entry
packet spfsCreateDirEntResponse extends spfsResponse
{
};

// Remove a directory entry
packet spfsRemoveDirEntRequest extends spfsRequest
{
    fields:
        string entry;
};

// Remove a directory entry
packet spfsRemoveDirEntResponse extends spfsResponse
{
    fields:
        FSHandle removedHandle;
};

// Resolve a filesystem path
packet spfsChangeDirEntRequest extends spfsRequest
{
    fields:
        FSHandle parentHandle;
        FSHandle newHandle;
        string entry;
};

// Change a directory entry
packet spfsChangeDirEntResponse extends spfsResponse
{
    fields:
        FSHandle oldDirEntHandle;
};

// Resolve a filesystem path
packet spfsTruncateRequest extends spfsRequest
{
    fields:
        FSSize newSize;
        int flags;
};

// Truncate a data object
packet spfsTruncateResponse extends spfsResponse
{
};

// Resolve a filesystem path
packet spfsMakeDirRequest extends spfsRequest
{
    fields:
        FSSize handleExtents[];
};

// Create a directory
packet spfsMakeDirResponse extends spfsResponse
{
    fields:
};

// Resolve a filesystem path
packet spfsReadDirRequest extends spfsRequest
{
    fields:
        FSOffset dirOffset;
        unsigned long dirEntCount;
};

// Read a directory
packet spfsReadDirResponse extends spfsResponse
{
    fields:
        FSOffset dirOffset;
        string entries[];
        unsigned long entryCount;
        unsigned long directoryVersion;
};

// Resolve a filesystem path
packet spfsFlushRequest extends spfsRequest
{
    fields:
        string path;
};

// Flush a data object
packet spfsFlushResponse extends spfsResponse
{
};

// Resolve a parallel filesystem path
packet spfsStatRequest extends spfsRequest
{
    fields:
        int flags;
};

// Stat parallel filesystem
packet spfsStatResponse extends spfsResponse
{
    fields:
        string stat;
};

// Resolve a parallel filesystem path
packet spfsListAttrRequest extends spfsRequest
{
    fields:
        FSHandle handles[];
        unsigned long handleCount;
        int attrMask;
};

// List data object attributes
packet spfsListAttrResponse extends spfsResponse
{
    fields:
        unsigned long numHandles;
};

// Begin the data flow
packet spfsDataFlowStart extends spfsRequest
{
    fields:
		// File information
		FSSize bstreamSize;
        
        // Flow information
		int flowType;
        int flowMode;

		// Flow info set by job manager
        int flowId;

        // The bmi tags to use for this flow
        int inboundBmiTag;
        int outboundBmiTag;
};

packet spfsClientDataFlowStart extends spfsDataFlowStart
{
	fields:
        FSOffset offset;
        DataTypePtr dataType;
        unsigned long count;
        FileViewPtr view;
        FileDistributionPtr dist;

        // Client Data information
        VoidPtr clientContextPointer;
}

packet spfsServerDataFlowStart extends spfsDataFlowStart
{
	fields:
		FSHandle metaHandle;
        FSOffset offset;
        FSSize dataSize;
        FileViewPtr view;
        FileDistributionPtr dist;
}

// Indicate the data flow has finished
packet spfsDataFlowFinish extends spfsResponse
{
    fields:
        int flowId;
        FSSize flowSize;
};

// Create a file system object
packet spfsCollectiveCreateRequest extends spfsRequest
{
    fields:
        FSObjectType objectType;
        FSHandle metaHandle;
        FSHandle dataHandles[];
        int numOutstandingRequests;
};

// Create a file system object
packet spfsCollectiveCreateResponse extends spfsResponse
{
    fields:
        FSHandle handle;
};

// Get attributes a file system object
packet spfsCollectiveGetAttrRequest extends spfsRequest
{
    fields:
        FSObjectType objectType;
        FSHandle dataHandles[];
        int numOutstandingRequests;
};

// Get attributes response a file system object
packet spfsCollectiveGetAttrResponse extends spfsResponse
{
};

// Remove a file system object
packet spfsCollectiveRemoveRequest extends spfsRequest
{
    fields:
        FSObjectType objectType;
        FSHandle metaHandle;
        FSHandle dataHandles[];
        int numOutstandingRequests;
};

// Remove a file system object
packet spfsCollectiveRemoveResponse extends spfsResponse
{
};

// Flow pages
packet spfsCacheDataFlowStart extends spfsDataFlowStart
{
	fields:
		unsigned long numPages;
        FSSize pageSize;

        // Client Data information
        VoidPtr clientContextPointer;
}

// Read object data
packet spfsInvalidatePagesRequest extends spfsRequest
{
    fields:
    	bool isExclusive;
    	bool forwardToDestination;
    	int destinationRank;

		string filename;
    	FSSize pageSize;
    	int pageIds[];    	
};

// Read file data
packet spfsInvalidatePagesWrapperRequest extends spfsResponse
{
    fields:
};

// Read object data
packet spfsReadPagesRequest extends spfsReadRequest
{
    fields:
    	bool isExclusive;
    	int originatingRank;
    	string filename;
    	FSSize pageSize;
};

// Read file data
packet spfsReadPagesResponse extends spfsReadResponse
{
    fields:
    	FSSize pageSize;
    	unsigned long clientPageIds[];
    	unsigned long serverPageIds[];
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
