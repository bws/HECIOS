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
#include <pfs_types.h>

typedef FSOpenFile* FileDescriptorPtr;

/** MPI Data type */
typedef FSDataType MPIDataType;
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

enum mpiRequestKind
{
    SPFS_MPI_SEND_REQUEST = 1;
    SPFS_MPI_FILE_OPEN_REQUEST = 101;
    SPFS_MPI_FILE_CLOSE_REQUEST = 102;
    SPFS_MPI_FILE_DELETE_REQUEST = 103;
    SPFS_MPI_FILE_SET_SIZE_REQUEST = 104;
    SPFS_MPI_FILE_PREALLOCATE_REQUEST = 105;
    SPFS_MPI_FILE_GET_SIZE_REQUEST = 106;
    SPFS_MPI_FILE_GET_INFO_REQUEST = 107;
    SPFS_MPI_FILE_SET_INFO_REQUEST = 108;
    SPFS_MPI_FILE_READ_AT_REQUEST = 109;
    SPFS_MPI_FILE_READ_REQUEST = 110;
    SPFS_MPI_FILE_WRITE_AT_REQUEST = 111;
    SPFS_MPI_FILE_WRITE_REQUEST = 112;

    //MPI COLLECTIVE REQUEST MSGS
    SPFS_MPI_BCAST_REQUEST = 2;
};

enum mpiResponseKind
{
    SPFS_MPI_SEND_RESPONSE = 201;
    SPFS_MPI_FILE_OPEN_RESPONSE = 301;
    SPFS_MPI_FILE_CLOSE_RESPONSE = 302;
    SPFS_MPI_FILE_DELETE_RESPONSE = 303;
    SPFS_MPI_FILE_SET_SIZE_RESPONSE = 304;
    SPFS_MPI_FILE_PREALLOCATE_RESPONSE = 305;
    SPFS_MPI_FILE_GET_SIZE_RESPONSE = 306;
    SPFS_MPI_FILE_GET_INFO_RESPONSE = 307;
    SPFS_MPI_FILE_SET_INFO_RESPONSE = 308;
    SPFS_MPI_FILE_READ_AT_RESPONSE = 309;
    SPFS_MPI_FILE_READ_RESPONSE = 310;
    SPFS_MPI_FILE_WRITE_AT_RESPONSE = 311;
    SPFS_MPI_FILE_WRITE_RESPONSE = 312;

    //MPI COLLECTIVE RESPONSE MSGS
    SPFS_MPI_BCAST_RESPONSE = 202;
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
        int responses = 0;
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
};

// File read response
message spfsMPIFileReadResponse extends spfsMPIResponse
{
    fields:
        char data[];
        unsigned long bytesRead;
};

// Request to read at a file offset
message spfsMPIFileReadAtRequest extends spfsMPIFileReadRequest
{
    fields:
        long offset;
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
        char data[];
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
};

// File writeAt response
message spfsMPIFileWriteAtResponse extends spfsMPIFileWriteResponse
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
