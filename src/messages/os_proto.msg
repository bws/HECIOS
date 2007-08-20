//
// Messages for interacting with the OS layer
//

// Pull in the correct types for block addressing
cplusplus {{
#include "basic_types.h"
}}

// Forward declarations
class noncobject FSBlock;

// Enumerate all OS message kinds
enum spfsOSMessageKind
{
    SPFS_OS_FILE_READ_REQUEST = 601;
    SPFS_OS_FILE_READ_RESPONSE = 602;
    SPFS_OS_FILE_WRITE_REQUEST = 603;
    SPFS_OS_FILE_WRITE_RESPONSE = 604;
    SPFS_OS_READ_BLOCKS_REQUEST = 605;
    SPFS_OS_READ_BLOCKS_RESPONSE = 606;
    SPFS_OS_WRITE_BLOCKS_REQUEST = 607;
    SPFS_OS_WRITE_BLOCKS_RESPONSE = 608;
    SPFS_OS_READ_DEVICE_REQUEST = 609;
    SPFS_OS_READ_DEVICE_RESPONSE = 610;
    SPFS_OS_WRITE_DEVICE_REQUEST = 611;
    SPFS_OS_WRITE_DEVICE_RESPONSE = 612;
};

// The abstract base class for all OS File Read and File Write Requests
message spfsOSFileRequest
{
    fields:
        long fileHandle;
};

// The abstract base class for all OF File Read and File Write Responses
message spfsOSFileResponse
{
    fields:
        long fileHandle;
};

// File read request to the OS' native file system
message spfsOSFileReadRequest extends spfsOSFileRequest
{
    fields:
        long offset;
        long extent;
};

// File Read response from the OS' native file system
message spfsOSFileReadResponse extends spfsOSFileResponse
{
};

// File write request to the OS' native file system
message spfsOSFileWriteRequest extends spfsOSFileRequest
{
    fields:
        long offset;
        long extent;
};

// File write response from the OS' native file system
message spfsOSReadBlocksRequest
{
    fields:
        FSBlock blocks[];
};

message spfsOSReadBlocksResponse
{
};

message spfsOSWriteBlocksRequest
{
    fields:
        FSBlock blocks[];
};

message spfsOSReadDeviceRequest
{
    fields:
        long address;
};

message spfsOSReadDeviceResponse
{
};

message spfsOSWriteDeviceRequest
{
    fields:
        long address;
};

message spfsOSWriteDeviceResponse
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
