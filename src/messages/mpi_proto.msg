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

//
// User request messages for MPI IO
//

// allow a pointer field in a message 
cplusplus {{
#include <data_type.h>
#include <file_descriptor.h>
#include <pfs_types.h>

typedef DataType* DataTypePtr;
typedef FileDescriptor* FileDescriptorPtr;

/** MPI Data type */
typedef DataTypePtr MPIDataType;
}}

class cFSM;
class noncobject MPIDataType;  // this is an int type
class noncobject FileDescriptorPtr;

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
    SPFS_MPI_BCAST_REQUEST = 12;
    SPFS_MPI_BCAST_RESPONSE = 13;

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
    SPFS_MPI_FILE_GET_SIZE_REQUEST = 110;
    SPFS_MPI_FILE_GET_SIZE_RESPONSE = 111;
    SPFS_MPI_FILE_GET_INFO_REQUEST = 112;
    SPFS_MPI_FILE_GET_INFO_RESPONSE = 113;
    SPFS_MPI_FILE_SET_INFO_REQUEST = 114;
    SPFS_MPI_FILE_SET_INFO_RESPONSE = 115;
    SPFS_MPI_FILE_READ_AT_REQUEST = 116;
    SPFS_MPI_FILE_READ_AT_RESPONSE = 117;
    SPFS_MPI_FILE_READ_REQUEST = 118;
    SPFS_MPI_FILE_READ_RESPONSE = 119;
    SPFS_MPI_FILE_WRITE_AT_REQUEST = 120;
    SPFS_MPI_FILE_WRITE_AT_RESPONSE = 121;
    SPFS_MPI_FILE_WRITE_REQUEST = 122;
    SPFS_MPI_FILE_WRITE_RESPONSE = 123;
    SPFS_MPI_FILE_IREAD_REQUEST = 124;
    SPFS_MPI_FILE_IREAD_RESPONSE = 125;
    SPFS_MPI_FILE_IWRITE_REQUEST = 126;
    SPFS_MPI_FILE_IWRITE_RESPONSE = 127;

    // MPI Extensions for our simulator
    SPFS_MPI_DIRECTORY_CREATE_REQUEST = 200;
    SPFS_MPI_DIRECTORY_CREATE_RESPONSE = 201;
    SPFS_MPI_FILE_UPDATE_TIME_REQUEST = 202;
    SPFS_MPI_FILE_UPDATE_TIME_RESPONSE = 203;
    SPFS_MPI_FILE_STAT_REQUEST = 204;
    SPFS_MPI_FILE_STAT_RESPONSE = 205;
};

// The abstract base class for all MPI message requests
message spfsMPIRequest
{
    fields:
        bool isCollective = false;
        int rank;
        int commMembers[] = 0;

        // internal fields
        cFSM state;
        int remainingResponses = 0;
        int socketId;
};

// The abstract base class for all MPI message responses
message spfsMPIResponse
{
    fields:
        bool isSuccessful;
};

// Simple MPI Send request
message spfsMPISendRequest extends spfsMPIRequest
{
};

// Simple MPI Send response
message spfsMPISendResponse extends spfsMPIResponse
{
};

// Request to open a file 
message spfsMPIFileOpenRequest extends spfsMPIRequest
{
    fields:
        string fileName;
        int mode;

        // internal fields
        FileDescriptorPtr fileDes;
        unsigned int numResolvedSegments = 1;
};

// Open file response
message spfsMPIFileOpenResponse extends spfsMPIResponse
{
    fields:
        FileDescriptorPtr fileDes;
};

// Request to close a file
message spfsMPIFileCloseRequest extends spfsMPIRequest
{
    fields:
        FileDescriptorPtr fileDes;
};

// Close file response
message spfsMPIFileCloseResponse extends spfsMPIResponse
{
    fields:
};

// Request to delete a file
message spfsMPIFileDeleteRequest extends spfsMPIRequest
{
    fields:
        string fileName;
};

// Delete file response
message spfsMPIFileDeleteResponse extends spfsMPIResponse
{
};

// Request to resize a file
message spfsMPIFileSetSizeRequest extends spfsMPIRequest
{
    fields:
        FileDescriptorPtr fileDes;
        int fileSize;
};

// Resize file response
message spfsMPIFileSetSizeResponse extends spfsMPIResponse
{
};

// Request to preallocate file space
message spfsMPIFilePreallocateRequest extends spfsMPIRequest
{
    fields:
        FileDescriptorPtr fileDes;
        int fileSize;
};

// Delete file response
message spfsMPIFilePreallocateResponse extends spfsMPIResponse
{
};

// Request to get the size of a file
message spfsMPIFileGetSizeRequest extends spfsMPIRequest
{
    fields:
        string fileName;
};

// Get file size response
message spfsMPIFileGetSizeResponse extends spfsMPIResponse
{
    fields:
        int fileSize;
};

// Request to get the file info object
message spfsMPIFileGetInfoRequest extends spfsMPIRequest
{
    fields:
        FileDescriptorPtr fileDes;
};

// Get file info response
message spfsMPIFileGetInfoResponse extends spfsMPIResponse
{
    fields:
        string info[];
};

// Request to set the file info object
message spfsMPIFileSetInfoRequest extends spfsMPIRequest
{
    fields:
        FileDescriptorPtr fileDes;
        string info[];
};

// Set file info response
message spfsMPIFileSetInfoResponse extends spfsMPIResponse
{
};

// Request to read from a file
message spfsMPIFileReadRequest extends spfsMPIRequest
{
    fields:
	FileDescriptorPtr fileDes;
	int count;
	MPIDataType dataType;

        // Internal fields used during processing
        int remainingFlows;
};

// File read response
message spfsMPIFileReadResponse extends spfsMPIResponse
{
    fields:
        unsigned long bytesRead;
};

// Request to read at a file offset
message spfsMPIFileReadAtRequest extends spfsMPIFileReadRequest
{
    fields:
        long offset;
        long reqId;
};

// File readAt response
message spfsMPIFileReadAtResponse extends spfsMPIFileReadResponse
{
};

// Request to write to a file
message spfsMPIFileWriteRequest extends spfsMPIRequest
{
    fields:
        FileDescriptorPtr fileDes;
        int count;
        MPIDataType dataType;

        // Internal fields used during processing
        int remainingFlows;
        int remainingCompletions;
};

// File write response
message spfsMPIFileWriteResponse extends spfsMPIResponse
{
    fields:
        unsigned long bytesWritten;
};

// Request to write at a file offset
message spfsMPIFileWriteAtRequest extends spfsMPIFileWriteRequest
{
    fields:
        long offset;
        long reqId;
};

// File writeAt response
message spfsMPIFileWriteAtResponse extends spfsMPIFileWriteResponse
{
};

// Request to create a directory
message spfsMPIDirectoryCreateRequest extends spfsMPIRequest
{
    fields:
        string dirName;

        // internal fields
        unsigned int numResolvedSegments = 1;
};

// Directory creation response
message spfsMPIDirectoryCreateResponse extends spfsMPIResponse
{
};

// Request to update a file's atime
message spfsMPIFileUpdateTimeRequest extends spfsMPIRequest
{
    fields:
        string fileName;

        // internal fields
        unsigned int numResolvedSegments = 1;
};

// Response to atime update
message spfsMPIFileUpdateTimeResponse extends spfsMPIResponse
{
};

// Broadcast request
message spfsMPIBcastRequest extends spfsMPIRequest
{
    fields:	
        long root;
        bool isGlobal;
};

// Broadcast response
message spfsMPIBcastResponse extends spfsMPIResponse
{
};



//
// Local variables:
//  c-indent-level: 4
//  c-basic-offset: 4
// End:
//
// vim: ts=4 sts=4 sw=4 expandtab foldmethod=marker
//
