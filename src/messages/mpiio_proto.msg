//
// User request messages for MPI IO
//

// allow a pointer field in a message 
cplusplus {{
#include <pfs_types.h>

typedef FSOpenFile* FileDescriptorPtr;

/** MPI Data type -- currently indicates number of bytes */
typedef int MPIDataType;

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

enum mpiRequestType
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

enum mpiResponseType
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
        string fileName;
        int mode;

        // internal fields
        FileDescriptorPtr filedes;
};

// Open file response
message spfsMPIFileOpenResponse extends spfsMPIResponse
{
    fields:
        FileDescriptorPtr filedes;
};

// Request to close a file
message spfsMPIFileCloseRequest extends spfsMPIRequest
{
    fields:
        FileDescriptorPtr filedes;
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
        FileDescriptorPtr filedes;
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
        FileDescriptorPtr filedes;
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
        FileDescriptorPtr filedes;
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
        FileDescriptorPtr filedes;
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
	FileDescriptorPtr filedes;
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
        unsigned long bytesRead;
};

// Request to read from a file
message spfsMPIFileReadRequest extends spfsMPIRequest
{
    fields:
	FileDescriptorPtr filedes;
	int count;
	MPIDataType dtype;
};

// File read response
message spfsMPIFileReadResponse extends spfsMPIResponse
{
    fields:
        char data[];
        unsigned long bytes_read;
        unsigned long bytesRead;
};

// Request to write at a file offset
message spfsMPIFileWriteAtRequest extends spfsMPIRequest
{
    fields:
        FileDescriptorPtr filedes;
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
        FileDescriptorPtr filedes;
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

//
// Local variables:
//  c-indent-level: 4
//  c-basic-offset: 4
// End:
//
// vim: ts=4 sts=4 sw=4 expandtab foldmethod=marker
//
