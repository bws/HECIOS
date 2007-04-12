//
// User request messages for MPI IO
//

// allow a pointer field in a message 
cplusplus {{
#include <pfs_types.h>
typedef void *voidPtr;
}}

class cFSM;
class noncobject voidPtr;
class noncobject MPIDataType;  // this is an int type

enum mpiRequestTypes
{
    SPFS_MPI_FILE_OPEN_REQUEST = 1;
    SPFS_MPI_FILE_CLOSE_REQUEST = 2;
    SPFS_MPI_FILE_DELETE_REQUEST = 3;
    SPFS_MPI_FILE_SET_SIZE_REQUEST = 4;
    SPFS_MPI_FILE_PREALLOCATE_REQUEST = 5;
    SPFS_MPI_FILE_GET_SIZE_REQUEST = 6;
    SPFS_MPI_FILE_GET_INFO_REQUEST = 7;
    SPFS_MPI_FILE_SET_INFO_REQUEST = 8;
    SPFS_MPI_FILE_READ_AT_REQUEST = 9;
    SPFS_MPI_FILE_READ_REQUEST = 10;
    SPFS_MPI_FILE_WRITE_AT_REQUEST = 11;
    SPFS_MPI_FILE_WRITE_REQUEST = 12;
};

enum mpiResponseTypes
{
    SPFS_MPI_FILE_OPEN_RESPONSE = 101;
    SPFS_MPI_FILE_CLOSE_RESPONSE = 102;
    SPFS_MPI_FILE_DELETE_RESPONSE = 103;
    SPFS_MPI_FILE_SET_SIZE_RESPONSE = 104;
    SPFS_MPI_FILE_PREALLOCATE_RESPONSE = 105;
    SPFS_MPI_FILE_GET_SIZE_RESPONSE = 106;
    SPFS_MPI_FILE_GET_INFO_RESPONSE = 107;
    SPFS_MPI_FILE_SET_INFO_RESPONSE = 108;
    SPFS_MPI_FILE_READ_AT_RESPONSE = 109;
    SPFS_MPI_FILE_READ_RESPONSE = 110;
    SPFS_MPI_FILE_WRITE_AT_RESPONSE = 111;
    SPFS_MPI_FILE_WRITE_RESPONSE = 112;
};

// The abstract base class for all MPI IO message requests
message spfsMPIRequest
{
    fields:
        bool isCollective = false;
        int rank = 0;
        int size = 1;
        int comm_members[] = 0;
		  // internal fields
		  cFSM state;
		  int responses = 0;
};

// The abstract base class for all MPI IO message responses
message spfsMPIResponse
{
    fields:
        bool isSuccessful;
};

// Request to open a file 
message spfsMPIFileOpenRequest extends spfsMPIRequest
{
    fields:
	     voidPtr fs;    // pointer to fileSystem class 
        string fileName;
        int mode;
		  // internal fields
		  voidPtr filedes;
};

// Open file response
message spfsMPIFileOpenResponse extends spfsMPIResponse
{
    fields:
		  voidPtr filedes;
};

// Request to close a file
message spfsMPIFileCloseRequest extends spfsMPIRequest
{
    fields:
		  voidPtr filedes;
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
		  voidPtr filedes;
        int size;
};

// Resize file response
message spfsMPIFileSetSizeResponse extends spfsMPIResponse
{
};

// Request to preallocate file space
message spfsMPIFilePreallocateRequest extends spfsMPIRequest
{
    fields:
		  voidPtr filedes;
        int size;
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
		  voidPtr filedes;
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
		  voidPtr filedes;
        string info[];
};

// Set file info response
message spfsMPIFileSetInfoResponse extends spfsMPIResponse
{
};

// Request to read at a file offset
message spfsMPIFileReadAtRequest extends spfsMPIRequest
{
    fields:
	voidPtr filedes;
	int count;
	MPIDataType dtype;
        long offset = 0;
};

// File readAt response
message spfsMPIFileReadAtResponse extends spfsMPIResponse
{
    fields:
        char data[];
        unsigned long bytes_read;
};

// Request to read from a file
message spfsMPIFileReadRequest extends spfsMPIRequest
{
    fields:
	voidPtr filedes;
	int count;
	MPIDataType dtype;
};

// File read response
message spfsMPIFileReadResponse extends spfsMPIResponse
{
    fields:
        char data[];
        unsigned long bytes_read;
};

// Request to write at a file offset
message spfsMPIFileWriteAtRequest extends spfsMPIRequest
{
    fields:
		  voidPtr filedes;
	     int count;
	     MPIDataType dtype;
        long offset;
        char data[];
};

// File writeAt response
message spfsMPIFileWriteAtResponse extends spfsMPIResponse
{
    fields:
        unsigned long bytes_written;
};

// Request to write to a file
message spfsMPIFileWriteRequest extends spfsMPIRequest
{
    fields:
		  voidPtr filedes;
	     int count;
	     MPIDataType dtype;
        char data[];
};

// File write response
message spfsMPIFileWriteResponse extends spfsMPIResponse
{
    fields:
        unsigned long bytes_written;
};

