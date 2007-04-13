//
// BMI wrapper messages for transporting data from the client to server
//

enum spfsOSMessageTypes
{
    SPFS_OS_FILE_READ_REQUEST = 601;
    SPFS_OS_FILE_READ_RESPONSE = 602;
    SPFS_OS_FILE_WRITE_REQUEST = 603;
    SPFS_OS_FILE_WRITE_RESPONSE = 604;
};

// The abstract base class for all OS File Read and Write Requests
message spfsOSFileRequest
{
    fields:
        long fileHandle;
        long offset;
        long extent;
        long jobId;
};

// The abstract base class for all OF File Read and Write Responses
message spfsOSFileResponse
{
    fields:
        long fileHandle;
        long jobId;
};

// File read request to the OS' native file system
message spfsOSFileReadRequest extends spfsOSFileRequest
{
};

// File Read response from the OS' native file system
message spfsOSFileReadResponse extends spfsOSFileResponse
{
};

// File write request to the OS' native file system
message spfsOSFileWriteRequest extends spfsOSFileRequest
{
};

// File write response from the OS' native file system
message spfsOSFileWriteResponse extends spfsOSFileResponse
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
