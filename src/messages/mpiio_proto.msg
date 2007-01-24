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

// Request to delete a file
message mpiFilePreallocateRequest
{
    fields:
        string fileName;
};

// Delete file response
message mpiFilepreallocateResponse
{
};

// Request to delete a file
message mpiFileGetSizeRequest
{
    fields:
        string fileName;
};

// Delete file response
message mpiFileGetSizeResponse
{
};

// Resize file response
message mpiFileSetSizeResponse
{
};

// Request to read from a file
message mpiFileReadRequest
{
    fields:
        int handle;
        long offset = 0;
};

// File read response
message mpiFileReadResponse
{
    fields:
        char data[];
        unsigned long bytes_read;
        bool isblocking = false;
};

// Request to write to a file
message mpiFileWriteRequest
{
    fields:
        int handle;
        char data[];
        unsigned long num_bytes;
        long offset = 0;
        bool isBlocking = false;
};

// File write response
message mpiFileWriteResponse
{
    fields:
        unsigned long bytes_written;
};

