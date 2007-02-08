//
// User request messages for MPI IO
//

enum mpiRequestTypes
{
    MPI_FILE_OPEN_REQUEST = 1;
    MPI_FILE_CLOSE_REQUEST = 2;
    MPI_FILE_DELETE_REQUEST = 3;
    MPI_FILE_SET_SIZE_REQUEST = 4;
    MPI_FILE_PREALLOCATE_REQUEST = 5;
    MPI_FILE_GET_SIZE_REQUEST = 6;
    MPI_FILE_GET_INFO_REQUEST = 7;
    MPI_FILE_SET_INFO_REQUEST = 8;
    MPI_FILE_READ_AT_REQUEST = 9;
    MPI_FILE_READ_REQUEST = 10;
    MPI_FILE_WRITE_AT_REQUEST = 11;
    MPI_FILE_WRITE_REQUEST = 12;
};

enum mpiResponseTypes
{
    MPI_FILE_OPEN_RESPONSE = 101;
    MPI_FILE_CLOSE_RESPONSE = 102;
    MPI_FILE_DELETE_RESPONSE = 103;
    MPI_FILE_SET_SIZE_RESPONSE = 104;
    MPI_FILE_PREALLOCATE_RESPONSE = 105;
    MPI_FILE_GET_SIZE_RESPONSE = 106;
    MPI_FILE_GET_INFO_RESPONSE = 107;
    MPI_FILE_SET_INFO_RESPONSE = 108;
    MPI_FILE_READ_AT_RESPONSE = 109;
    MPI_FILE_READ_RESPONSE = 110;
    MPI_FILE_WRITE_AT_RESPONSE = 111;
    MPI_FILE_WRITE_RESPONSE = 112;
};

// The abstract base class for all MPI IO message requests
message mpiRequest
{
    fields:
        bool isCollective = false;
        int rank = 0;
        int size = 1;
        int comm_members[] = 0;
};

// The abstract base class for all MPI IO message responses
message mpiResponse
{
    fields:
        bool isSuccessful;
};

// Request to open a file 
message mpiFileOpenRequest extends mpiRequest
{
    fields:
        string fileName;
        int mode;
};

// Open file response
message mpiFileOpenResponse extends mpiResponse
{
    fields:
        string fileName;
        int mode;
};

// Request to close a file
message mpiFileCloseRequest extends mpiRequest
{
    fields:
        int handle;
};

// Close file response
message mpiFileCloseResponse extends mpiResponse
{
    fields:
        int handle;
};

// Request to delete a file
message mpiFileDeleteRequest extends mpiRequest
{
    fields:
        string fileName;
};

// Delete file response
message mpiFileDeleteResponse extends mpiResponse
{
};

// Request to resize a file
message mpiFileSetSizeRequest extends mpiRequest
{
    fields:
        int handle;
        int size;
};

// Resize file response
message mpiFileSetSizeResponse extends mpiResponse
{
};

// Request to preallocate file space
message mpiFilePreallocateRequest extends mpiRequest
{
    fields:
        int handle;
        int size;
};

// Delete file response
message mpiFilepreallocateResponse extends mpiResponse
{
};

// Request to get the size of a file
message mpiFileGetSizeRequest extends mpiRequest
{
    fields:
        string fileName;
};

// Get file size response
message mpiFileGetSizeResponse
{
};

// Request to get the file info object
message mpiFileGetInfoRequest extends mpiRequest
{
    fields:
        int handle;
};

// Get file info response
message mpiFileGetInfoResponse extends mpiResponse
{
    fields:
        string info[];
};

// Request to set the file info object
message mpiFileSetInfoRequest extends mpiRequest
{
    fields:
        int handle;
        string info[];
};

// Set file info response
message mpiFileSetInfoResponse extends mpiResponse
{
};

// Request to read at a file offset
message mpiFileReadAtRequest extends mpiRequest
{
    fields:
        int handle;
		  int count;
		  mpiDatatype dtype;
        long offset = 0;
};

// File readAt response
message mpiFileReadAtResponse extends mpiResponse
{
    fields:
        char data[];
        unsigned long bytes_read;
};

// Request to read from a file
message mpiFileReadRequest extends mpiRequest
{
    fields:
        int handle;
		  int count;
		  mpiDatatype dtype;
};

// File read response
message mpiFileReadResponse extends mpiResponse
{
    fields:
        char data[];
        unsigned long bytes_read;
};

// Request to write at a file offset
message mpiFileWriteAtRequest extends mpiRequest
{
    fields:
        int handle;
		  int count;
		  mpiDatatype dtype;
        long offset;
        char data[];
};

// File writeAt response
message mpiFileWriteAtResponse extends mpiResponse
{
    fields:
        unsigned long bytes_written;
};

// Request to write to a file
message mpiFileWriteRequest extends mpiRequest
{
    fields:
        int handle;
		  int count;
		  mpiDatatype dtype;
        char data[];
};

// File write response
message mpiFileWriteResponse extends mpiResponse
{
    fields:
        unsigned long bytes_written;
};

