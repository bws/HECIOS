//
// User request messages for MPI IO
//

enum mpiRequestTypes
{
	mpiFileOpenRequest = 1;
   mpiFileCloseRequest = 2;
   mpiFileDeleteRequest = 3;
   mpiFileSetSizeRequest = 4;
   mpiFilePreallocateRequest = 5;
   mpiFileGetSizeRequest = 6;
   mpiFileGetInfoRequest = 7;
   mpiFileSetInfoRequest = 8;
   mpiFileReadAtRequest = 9;
   mpiFileReadRequest = 10;
   mpiFileWriteAtRequest = 11;
   mpiFileWriteRequest = 12;
};

enum mpiResponseTypes
{
   mpiFileOpenResponse = 21;
   mpiFileCloseResponse = 22;
   mpiFileDeleteResponse = 23;
   mpiFileSetSizeResponse = 24;
   mpiFilepreallocateResponse = 25;
   mpiFileGetSizeResponse = 26;
   mpiFileGetInfoResponse = 27;
   mpiFileSetInfoResponse = 28;
   mpiFileReadAtResponse = 29;
   mpiFileReadResponse = 30;
   mpiFileWriteAtResponse = 31;
   mpiFileWriteResponse = 31;
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
	     int mpiRequestType enum(mpiRequestTypes) = mpiFileOpenRequest;
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
        int mpiRequestType enum(mpiRequestTypes) = mpiFileCloseRequest;
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
        int mpiRequestType enum(mpiRequestTypes) = mpiFileDeleteRequest;
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
        int mpiRequestType enum(mpiRequestTypes) = mpiFileSetSizeRequest;
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
        int mpiRequestType enum(mpiRequestTypes) = mpiFilePreallocateRequest;
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
        int mpiRequestType enum(mpiRequestTypes) = mpiFileGetSizeRequest;
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
        int mpiRequestType enum(mpiRequestTypes) = mpiFileGetInfoRequest;
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
        int mpiRequestType enum(mpiRequestTypes) = mpiFileSetInfoRequest;
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
        int mpiRequestType enum(mpiRequestTypes) = mpiFileReadAtRequest;
        int handle;
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
        int mpiRequestType enum(mpiRequestTypes) = mpiFileReadRequest;
        int handle;
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
        int mpiRequestType enum(mpiRequestTypes) = mpiFileWriteAtRequest;
        int handle;
        char data[];
        unsigned long num_bytes;
        long offset;
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
        int mpiRequestType enum(mpiRequestTypes) = mpiFileWriteRequest;
        int handle;
        char data[];
        unsigned long num_bytes;
};

// File write response
message mpiFileWriteResponse extends mpiResponse
{
    fields:
        unsigned long bytes_written;
};

