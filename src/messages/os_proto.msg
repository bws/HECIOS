//
// This file is part of Hecios
//
// Copyright (C) 2007,2008,2009 Brad Settlemyer
//
// This file is distributed WITHOUT ANY WARRANTY. See the file 'License.txt'
// for details on this and other legal matters.
//

//
// Messages for interacting with the OS layer
//

// Pull in the correct types for block addressing
cplusplus {{
#include "basic_types.h"
}}

// Forward declarations
class noncobject FSBlock;
class noncobject FSOffset;
class noncobject FSSize;
class cFSM;

// Enumerate all OS message kinds
enum spfsOSMessageKind
{
    SPFS_OS_FILE_OPEN_REQUEST = 602;
    SPFS_OS_FILE_OPEN_RESPONSE = 603;
    SPFS_OS_FILE_READ_REQUEST = 604;
    SPFS_OS_FILE_READ_RESPONSE = 605;
    SPFS_OS_FILE_WRITE_REQUEST = 606;
    SPFS_OS_FILE_WRITE_RESPONSE = 607;
    SPFS_OS_FILE_SYNC_REQUEST = 608;
    SPFS_OS_FILE_SYNC_RESPONSE = 609;
    SPFS_OS_FILE_UNLINK_REQUEST = 610;
    SPFS_OS_FILE_UNLINK_RESPONSE = 611;
    SPFS_OS_READ_BLOCKS_REQUEST = 612;
    SPFS_OS_READ_BLOCKS_RESPONSE = 613;
    SPFS_OS_WRITE_BLOCKS_REQUEST = 614;
    SPFS_OS_WRITE_BLOCKS_RESPONSE = 615;
    SPFS_OS_READ_DEVICE_REQUEST = 616;
    SPFS_OS_READ_DEVICE_RESPONSE = 617;
    SPFS_OS_WRITE_DEVICE_REQUEST = 618;
    SPFS_OS_WRITE_DEVICE_RESPONSE = 619;
    SPFS_OS_SYNC_REQUEST = 620;
    SPFS_OS_SYNC_RESPONSE = 621;
};

// Abstract base class for all OS File requests
message spfsOSFileRequest
{
    fields:
        string filename;

        // internal fields
        cFSM state;
}

// Abstract base class for all OS File requests
message spfsOSFileResponse
{
    fields:
        string filename;
}

// Abstract base class for all OS UNIX File Read and File Write Requests
message spfsOSFileIORequest extends spfsOSFileRequest
{
    fields:
        FSOffset offset;
        FSSize extent;

        // State fields used internally
        bool hasMetaDataLoaded;
        unsigned long numRemainingResponses;
};

// Abstract base class for all OS File Read and File Write Responses
message spfsOSFileIOResponse extends spfsOSFileResponse
{
};

// Abstract base class for all OS List I/O File Read and File Write Requests
message spfsOSFileLIORequest extends spfsOSFileRequest
{
    fields:
        FSOffset offset[];
        FSSize extent[];

        // State fields used internally
        bool hasMetaDataLoaded;
        unsigned long numRemainingResponses;
};

// Abstract base class for all OS List I/O File Read and File Write Responses
message spfsOSFileLIOResponse extends spfsOSFileResponse
{
};

// File open request to the OS' native file system
message spfsOSFileOpenRequest extends spfsOSFileRequest
{
    fields:
        bool isCreate;
};

// File open response from the OS' native file system
message spfsOSFileOpenResponse extends spfsOSFileResponse
{
};

// File read request to the OS' native file system
message spfsOSFileReadRequest extends spfsOSFileLIORequest
{
};

// File Read response from the OS' native file system
message spfsOSFileReadResponse extends spfsOSFileLIOResponse
{
    fields:
        FSSize bytesRead;
};

// File write request to the OS' native file system
message spfsOSFileWriteRequest extends spfsOSFileLIORequest
{
};

// File write response to the OS' native file system
message spfsOSFileWriteResponse extends spfsOSFileLIOResponse
{
    fields:
        FSSize bytesWritten;
};

// File sync request to the OS' native file system
message spfsOSFileSyncRequest extends spfsOSFileRequest
{
};

// File sync response from the OS' native file system
message spfsOSFileSyncResponse extends spfsOSFileResponse
{
};

// File unlink request to the OS' native file system
message spfsOSFileUnlinkRequest extends spfsOSFileRequest
{
};

// File unlink response from the OS' native file system
message spfsOSFileUnlinkResponse extends spfsOSFileResponse
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
    fields:
        bool writeThrough = false;
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
    fields:
        bool readThrough = false;
};

// Read block device response
message spfsOSReadDeviceResponse
{
};

// Write block device request
message spfsOSWriteDeviceRequest extends spfsOSDeviceIORequest
{
    fields:
        bool writeThrough = false;
};

// Write block device response
message spfsOSWriteDeviceResponse
{
};

// Sync system call request to the OS' native file system
message spfsOSSyncRequest
{
};

// Sync system call response from the OS' native file system
message spfsOSSyncResponse
{
};

//
// Local variables:
//  indent-tabs-mode: nil
//  c-indent-level: 4
//  c-basic-offset: 4
// End:
//
// vim: ts=4 sts=4 sw=4 expandtab foldmethod=marker
//
