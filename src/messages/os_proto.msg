//
// Messages for interacting with the OS layer
//

// Pull in the correct types for block addressing
cplusplus {{
#include "basic_types.h"
}}

// Forward declarations
class noncobject FSBlock;
class cFSM;

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

// Abstract base class for all OS File Read and File Write Requests
message spfsOSFileIORequest
{
    fields:
        long fileHandle;
        long offset;
        long extent;

        // State fields used internally
        long numRemainingResponses;
};

// Abstract base class for all OF File Read and File Write Responses
message spfsOSFileIOResponse
{
    fields:
        long fileHandle;
};

// File read request to the OS' native file system
message spfsOSFileReadRequest extends spfsOSFileIORequest
{
};

// File Read response from the OS' native file system
message spfsOSFileReadResponse extends spfsOSFileIOResponse
{
};

// File write request to the OS' native file system
message spfsOSFileWriteRequest extends spfsOSFileIORequest
{
};

// Abstract base class for block read and write requests
message spfsOSBlockIORequest
{
    fields:
        FSBlock blocks[];

        // State field used internally
        long numRemainingResponses;
};

// Abstract base class block read and write responses
message spfsOSBlockIOResponse
{
};

// Read request for file system blocks
message spfsOSReadBlocksRequest extends spfsOSBlockIORequest
{
};

// Read response for file system blocks
message spfsOSReadBlocksResponse extends spfsOSBlockIOResponse
{
};

// Write request for file system blocks 
message spfsOSWriteBlocksRequest extends spfsOSBlockIORequest
{
};

// Write response for file system blocks 
message spfsOSWriteBlocksResponse extends spfsOSBlockIOResponse
{
};

// Abstract base class for device I/O requests
message spfsOSDeviceIORequest
{
    fields:
        long address;
};

// Read block device request
message spfsOSReadDeviceRequest extends spfsOSDeviceIORequest
{
};

// Read block device response
message spfsOSReadDeviceResponse
{
};

// Write block device request
message spfsOSWriteDeviceRequest extends spfsOSDeviceIORequest
{
};

// Write block device response
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
