//
// This file is part of Hecios
//
// Copyright (C) 2007,2008,2009 Brad Settlemyer
//
// This file is distributed WITHOUT ANY WARRANTY. See the file 'License.txt'
// for details on this and other legal matters.
//

//
// User request messages for MPI IO
//

// allow a pointer field in a message 
cplusplus {{
#include <comm_man.h>
#include <data_type.h>
#include <file_descriptor.h>
#include <fs_operation_state.h>
#include <pfs_types.h>

typedef DataType* DataTypePtr;
typedef FileDescriptor* FileDescriptorPtr;

/** MPI Data type */
typedef DataTypePtr MPIDataType;
}}

class cFSM;
class noncobject Communicator;
class noncobject MPIDataType;
class noncobject FileDescriptorPtr;
class noncobject FSOperationState;

enum mpiAccessMode
{
    MPI_MODE_CREATE = 1;
    MPI_MODE_RDONLY = 2;
    MPI_MODE_WRONLY = 4;
    MPI_MODE_RDWR = 8;
    MPI_MODE_DELETE_ON_CLOSE = 16;
    MPI_MODE_UNIQUE_OPEN = 32;
    MPI_MODE_EXCL = 64;
    MPI_MODE_APPEND = 128;
    MPI_MODE_SEQUENTIAL = 256;
};

enum mpiMessageKind
{
    // MPI message passing messages
    SPFS_MPI_SEND_REQUEST = 10;
    SPFS_MPI_SEND_RESPONSE = 11;
    SPFS_MPI_BARRIER_REQUEST = 12;
    SPFS_MPI_BARRIER_RESPONSE = 13;
    SPFS_MPI_BCAST_REQUEST = 14;
    SPFS_MPI_BCAST_RESPONSE = 15;

    // MPI I/O Messages
    SPFS_MPI_FILE_OPEN_REQUEST = 100;
    SPFS_MPI_FILE_OPEN_RESPONSE = 101;
    SPFS_MPI_FILE_CLOSE_REQUEST = 102;
    SPFS_MPI_FILE_CLOSE_RESPONSE = 103;
    SPFS_MPI_FILE_DELETE_REQUEST = 104;
    SPFS_MPI_FILE_DELETE_RESPONSE = 105;
    SPFS_MPI_FILE_SET_SIZE_REQUEST = 106;
    SPFS_MPI_FILE_SET_SIZE_RESPONSE = 107;
    SPFS_MPI_FILE_PREALLOCATE_REQUEST = 108;
    SPFS_MPI_FILE_PREALLOCATE_RESPONSE = 109;
    SPFS_MPI_FILE_GET_AMODE_REQUEST = 110;
    SPFS_MPI_FILE_GET_AMODE_RESPONSE = 111;
    SPFS_MPI_FILE_GET_SIZE_REQUEST = 112;
    SPFS_MPI_FILE_GET_SIZE_RESPONSE = 113;
    SPFS_MPI_FILE_GET_INFO_REQUEST = 114;
    SPFS_MPI_FILE_GET_INFO_RESPONSE = 115;
    SPFS_MPI_FILE_SET_INFO_REQUEST = 116;
    SPFS_MPI_FILE_SET_INFO_RESPONSE = 117;
    SPFS_MPI_FILE_READ_AT_REQUEST = 118;
    SPFS_MPI_FILE_READ_AT_RESPONSE = 119;
    SPFS_MPI_FILE_READ_REQUEST = 120;
    SPFS_MPI_FILE_READ_RESPONSE = 121;
    SPFS_MPI_FILE_WRITE_AT_REQUEST = 122;
    SPFS_MPI_FILE_WRITE_AT_RESPONSE = 123;
    SPFS_MPI_FILE_WRITE_REQUEST = 124;
    SPFS_MPI_FILE_WRITE_RESPONSE = 125;

    // MPI Extensions for our simulator
    SPFS_MPI_DIRECTORY_CREATE_REQUEST = 200;
    SPFS_MPI_DIRECTORY_CREATE_RESPONSE = 201;
    SPFS_MPI_DIRECTORY_READ_REQUEST = 202;
    SPFS_MPI_DIRECTORY_READ_RESPONSE = 203;
    SPFS_MPI_DIRECTORY_REMOVE_REQUEST = 204;
    SPFS_MPI_DIRECTORY_REMOVE_RESPONSE = 205;
    SPFS_MPI_FILE_STAT_REQUEST = 206;
    SPFS_MPI_FILE_STAT_RESPONSE = 207;
    SPFS_MPI_FILE_UPDATE_TIME_REQUEST = 208;
    SPFS_MPI_FILE_UPDATE_TIME_RESPONSE = 209;
};

// The abstract base class for all MPI message requests
packet spfsMPIRequest
{
    fields:
    	long byteLength;
    	
        // internal fields
        cFSM cacheState;
        FSOperationState opState;
        unsigned int numResolvedSegments = 1;
        int remainingResponses = 0;
        int connectionId = -1;
};

// The abstract base class for collective messages -- not used purely correctly
// because multiple inheritance is not supported
packet spfsMPICollectiveRequest extends spfsMPIRequest
{
    fields:
        bool isCollective = false;
        int rank;
        Communicator communicator;        
}

// The abstract base class for all MPI message requests requiring a descriptor
packet spfsMPIFileRequest extends spfsMPICollectiveRequest
{
    fields:
        FileDescriptorPtr fileDes = 0;
};

// The abstract base class for all MPI message responses
packet spfsMPIResponse
{
    fields:
        bool isSuccessful;
};

// Request to open a file 
packet spfsMPIFileOpenRequest extends spfsMPIFileRequest
{
    fields:
        string fileName;
        int mode;
};

// Open file response
packet spfsMPIFileOpenResponse extends spfsMPIResponse
{
    fields:
        FileDescriptorPtr fileDes;
};

// Request to close a file
packet spfsMPIFileCloseRequest extends spfsMPIFileRequest
{
    fields:
};

// Close file response
packet spfsMPIFileCloseResponse extends spfsMPIResponse
{
    fields:
};

// Request to delete a file
packet spfsMPIFileDeleteRequest extends spfsMPIRequest
{
    fields:
        string fileName;
};

// Delete file response
packet spfsMPIFileDeleteResponse extends spfsMPIResponse
{
};

// Request to resize a file
packet spfsMPIFileSetSizeRequest extends spfsMPIFileRequest
{
    fields:
        int fileSize;
};

// Resize file response
packet spfsMPIFileSetSizeResponse extends spfsMPIResponse
{
};

// Request to preallocate file space
packet spfsMPIFilePreallocateRequest extends spfsMPIFileRequest
{
    fields:
        int fileSize;
};

// Preallocate file response
packet spfsMPIFilePreallocateResponse extends spfsMPIResponse
{
};

// Request to get the access mode of a file
packet spfsMPIFileGetAModeRequest extends spfsMPIFileRequest
{
    fields:
};

// Get file access mode response
packet spfsMPIFileGetAModeResponse extends spfsMPIResponse
{
    fields:
        int accessMode;
};

// Request to get the size of a file
packet spfsMPIFileGetSizeRequest extends spfsMPIFileRequest
{
    fields:
};

// Get file size response
packet spfsMPIFileGetSizeResponse extends spfsMPIResponse
{
    fields:
        int fileSize;
};

// Request to get the file info object
packet spfsMPIFileGetInfoRequest extends spfsMPIFileRequest
{
    fields:
};

// Get file info response
packet spfsMPIFileGetInfoResponse extends spfsMPIResponse
{
    fields:
        string info[];
};

// Request to set the file info object
packet spfsMPIFileSetInfoRequest extends spfsMPIFileRequest
{
    fields:
        string info[];
};

// Set file info response
packet spfsMPIFileSetInfoResponse extends spfsMPIResponse
{
};

// Request to read from a file
packet spfsMPIFileReadRequest extends spfsMPIFileRequest
{
    fields:
		int count;
		MPIDataType dataType;

    	// Internal fields used during processing
    	int remainingFlows = 0;
};

// File read response
packet spfsMPIFileReadResponse extends spfsMPIResponse
{
    fields:
        unsigned long bytesRead;
};

// Request to read at a file offset
packet spfsMPIFileReadAtRequest extends spfsMPIFileReadRequest
{
    fields:
        long offset;
        long reqId;
};

// File readAt response
packet spfsMPIFileReadAtResponse extends spfsMPIFileReadResponse
{
};

// Request to write to a file
packet spfsMPIFileWriteRequest extends spfsMPIFileRequest
{
    fields:
        int count;
        MPIDataType dataType;

        // Internal fields used during processing
        int remainingFlows = 0;
        int remainingCompletions = 0;
};

// File write response
packet spfsMPIFileWriteResponse extends spfsMPIResponse
{
    fields:
        unsigned long bytesWritten;
};

// Request to write at a file offset
packet spfsMPIFileWriteAtRequest extends spfsMPIFileWriteRequest
{
    fields:
        long offset;
        long reqId;
};

// File writeAt response
packet spfsMPIFileWriteAtResponse extends spfsMPIFileWriteResponse
{
};

// Request to create a directory
packet spfsMPIDirectoryCreateRequest extends spfsMPIRequest
{
    fields:
        string dirName;

        // internal fields
        unsigned int numResolvedSegments = 1;
};

// Directory creation response
packet spfsMPIDirectoryCreateResponse extends spfsMPIResponse
{
};

// Request to read a directory
packet spfsMPIDirectoryReadRequest extends spfsMPIFileRequest
{
    fields:
        unsigned long count;
};

// Directory remove response
packet spfsMPIDirectoryReadResponse extends spfsMPIResponse
{
};

// Request to remove a directory
packet spfsMPIDirectoryRemoveRequest extends spfsMPIRequest
{
    fields:
        string dirName;
};

// Directory remove response
packet spfsMPIDirectoryRemoveResponse extends spfsMPIResponse
{
};

// Request to stat a file
packet spfsMPIFileStatRequest extends spfsMPIRequest
{
    fields:
        string fileName;
        bool determineFileSize;
};

// Response to file stat
packet spfsMPIFileStatResponse extends spfsMPIResponse
{
    fields:
        unsigned long fileSize;
};

// Request to update a file's atime
packet spfsMPIFileUpdateTimeRequest extends spfsMPIRequest
{
    fields:
        string fileName;
};

// Response to atime update
packet spfsMPIFileUpdateTimeResponse extends spfsMPIResponse
{
};

// Simple MPI Send request
packet spfsMPISendRequest extends spfsMPICollectiveRequest
{
};

// Simple MPI Send response
packet spfsMPISendResponse extends spfsMPIResponse
{
};

// Barrier request
packet spfsMPIBarrierRequest extends spfsMPICollectiveRequest
{
    fields:	
        int root;
};

// Broadcast response
packet spfsMPIBarrierResponse extends spfsMPIResponse
{
};


// Broadcast request
packet spfsMPIBcastRequest extends spfsMPICollectiveRequest
{
    fields:	
        int root;
};

// Broadcast response
packet spfsMPIBcastResponse extends spfsMPIResponse
{
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
