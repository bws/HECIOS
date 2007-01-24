//
// User request messages for MPI IO
//

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
message mpiFileOpenRequest
{
    fields:
        string fileName;
        int mode;
};

// Open file response
message mpiFileOpenResponse
{
    fields:
        string fileName;
        int mode;
};

// Request to close a file
message mpiFileCloseRequest
{
    fields:
        int handle;
};

// Close file response
message mpiFileCloseResponse
{
    fields:
        int handle;
};

// Request to delete a file
message mpiFileDeleteRequest
{
    fields:
        string fileName;
};

// Delete file response
message mpiFileDeleteResponse
{
};

// Request to resize a file
message mpiFileSetSizeRequest
{
    fields:
        int handle;
        int size;
};

// Resize file response
message mpiFileSetSizeResponse
{
};

// Request to preallocate file space
message mpiFilePreallocateRequest
{
    fields:
        int handle;
        int size;
};

// Delete file response
message mpiFilepreallocateResponse
{
};

// Request to get the size of a file
message mpiFileGetSizeRequest
{
    fields:
        string fileName;
};

// Get file size response
message mpiFileGetSizeResponse
{
};

// Request to get the file info object
message mpiFileGetInfoRequest
{
    fields:
        int handle;
};

// Get file info response
message mpiFileGetInfoResponse
{
    fields:
        string info[];
};

// Request to set the file info object
message mpiFileSetInfoRequest
{
    fields:
        int handle;
        string info[];
};

// Set file info response
message mpiFileSetInfoResponse
{
};

// Request to read at a file offset
message mpiFileReadAtRequest
{
    fields:
        int handle;
        long offset = 0;
};

// File readAt response
message mpiFileReadAtResponse
{
    fields:
        char data[];
        unsigned long bytes_read;
};

// Request to read from a file
message mpiFileReadRequest
{
    fields:
        int handle;
};

// File read response
message mpiFileReadResponse
{
    fields:
        char data[];
        unsigned long bytes_read;
};

// Request to write at a file offset
message mpiFileWriteAtRequest
{
    fields:
        int handle;
        char data[];
        unsigned long num_bytes;
        long offset;
};

// File writeAt response
message mpiFileWriteAtResponse
{
    fields:
        unsigned long bytes_written;
};

// Request to write to a file
message mpiFileWriteRequest
{
    fields:
        int handle;
        char data[];
        unsigned long num_bytes;
};

// File write response
message mpiFileWriteResponse
{
    fields:
        unsigned long bytes_written;
};

