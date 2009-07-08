//
// This file is part of Hecios
//
// Copyright (C) 2009 Brad Settlemyer
//
// This file is distributed WITHOUT ANY WARRANTY. See the file 'License.txt'
// for details on this and other legal matters.
//

//
// Middleware cache consistency/control messages
//

// allow a pointer field in a message 
cplusplus {{
#include "basic_types.h"
#include "data_type.h"
#include "file_descriptor.h"
#include "fs_operation_state.h"

/** File descriptor type */
typedef FileDescriptor* FileDescriptorPtr;
typedef DataType* MPIDataType;
}}

class cFSM;
class noncobject ConnectionId;
class noncobject MPIDataType;
class noncobject FileDescriptorPtr;
class noncobject FSHandle;
class noncobject FSOffset;
class noncobject FSOperationState;
class noncobject FSSize;

enum spfsCacheMessageKind
{
    SPFS_CACHE_INVALIDATE_REQUEST = 501;
    SPFS_CACHE_INVALIDATE_RESPONSE = 502;
    SPFS_CACHE_READ_EXCLUSIVE_REQUEST = 503;
    SPFS_CACHE_READ_EXCLUSIVE_RESPONSE = 504;
    SPFS_CACHE_READ_SHARED_REQUEST = 505;
    SPFS_CACHE_READ_SHARED_RESPONSE = 506;
    SPFS_CACHE_SEND_PAGES = 507;
};

message spfsCacheRequest
{
    fields:
        FileDescriptorPtr descriptor = 0;
        FSHandle handle;

        // internal fields
        FSOperationState opState;
        int numRemainingPages;
        ConnectionId mpiConnectionId;
        int destinationRank;
}

message spfsCacheResponse
{
}

message spfsCacheInvalidateRequest extends spfsCacheRequest
{
    fields:
    	FSSize pageSize;
    	unsigned long pageIds[];
}

message spfsCacheInvalidateResponse extends spfsCacheResponse
{
    fields:
}

message spfsCacheReadRequest extends spfsCacheRequest
{
    fields:
    	// Info about sender
    	int originatingRank;
    	
    	// Request information
        long offset;
		int count;
		MPIDataType dataType;
		FSSize pageSize;
		int numPages;
		unsigned int requestPageIds[];
		
		// Data for internal use
		int remainingResponses;
		int remainingFlows;
		int remainingPages;
		unsigned long responseCachePageIds[];
		unsigned long responseServerPageIds[];
		
}

message spfsCacheReadResponse extends spfsCacheResponse
{
    fields:
    	string filename;
    	unsigned long pageIds[];
}

message spfsCacheReadExclusiveRequest extends spfsCacheReadRequest
{
	fields:
}

message spfsCacheReadExclusiveResponse extends spfsCacheReadResponse
{
}

message spfsCacheReadSharedRequest extends spfsCacheReadRequest
{
}

message spfsCacheReadSharedResponse extends spfsCacheReadResponse
{
}

message spfsCacheUpgradeRequest extends spfsCacheRequest
{
    fields:
    	FSSize pageSize;
    	unsigned long pageIds[];
}

message spfsCacheUpgradeResponse extends spfsCacheResponse
{
    fields:
}

message spfsCacheSendPages extends spfsCacheReadResponse
{
    fields:
}


//
// Local variables:
//  indent-tabs-mode: nil
//  c-indent-level: 4
//  c-basic-offset: 4
// End:
//
// vim: ts=4 sts=4 sw=4 expandtab
//
