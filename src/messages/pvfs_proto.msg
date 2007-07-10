//
// Parallel File System request proto message definitions
//

cplusplus {{
#include <pfs_types.h>

class TCPSocket;
typedef TCPSocket* TCPSocketPtr;
}}

struct FSMetaData;
struct HandleRange;
class cFSM;
class noncobject FSDataType;
class noncobject TCPSocketPtr;


// PVFS Request Protocol Messages
enum fsRequestType {
    SPFS_CREATE_REQUEST = 201;
    SPFS_REMOVE_REQUEST = 202;
    SPFS_READ_REQUEST = 203;
    SPFS_WRITE_REQUEST = 204;
    SPFS_GET_ATTR_REQUEST = 205;
    SPFS_SET_ATTR_REQUEST = 206;
    SPFS_LOOKUP_PATH_REQUEST = 207;
    SPFS_CREATE_DIR_ENT_REQUEST = 208;
    SPFS_REMOVE_DIR_ENT_REQUEST = 209;
    SPFS_CHANGE_DIR_ENT_REQUEST = 210;
    SPFS_TRUNCATE_REQUEST = 211;
    SPFS_MAKE_DIR_REQUEST = 212;
    SPFS_READ_DIR_REQUEST = 213;
    SPFS_FLUSH_REQUEST = 214;
    SPFS_STAT_REQUEST = 215;
    SPFS_LIST_ATTR_REQUEST = 216;
};

// PVFS Response Protocol Messages
enum fsResponseType {
    SPFS_CREATE_RESPONSE = 301;
    SPFS_REMOVE_RESPONSE = 302;
    SPFS_READ_RESPONSE = 303;
    SPFS_WRITE_RESPONSE = 304;
    SPFS_GET_ATTR_RESPONSE = 305;
    SPFS_SET_ATTR_RESPONSE = 306;
    SPFS_LOOKUP_PATH_RESPONSE = 307;
    SPFS_CREATE_DIR_ENT_RESPONSE = 308;
    SPFS_REMOVE_DIR_ENT_RESPONSE = 309;
    SPFS_CHANGE_DIR_ENT_RESPONSE = 310;
    SPFS_TRUNCATE_RESPONSE = 311;
    SPFS_MAKE_DIR_RESPONSE = 312;
    SPFS_READ_DIR_RESPONSE = 313;
    SPFS_WRITE_COMPLETION_RESPONSE = 314;
    SPFS_FLUSH_RESPONSE = 315;
    SPFS_STAT_RESPONSE = 316;
    SPFS_LIST_ATTR_RESPONSE = 317;
}

enum spfsLookupStatus {
    SPFS_FOUND = 1;
    SPFS_PARTIAL = 2;
    SPFS_NOTFOUND = 3;
}


// File request abstract base class
message spfsRequest
{
    fields:
        int fsId;
        int serverNo; // which logical server
        int handle;   // indicates server address and object

        // internal fields
        cFSM state;
        int socketId;
};

// File system response abstract base class
message spfsResponse
{
    fields:
        bool requestSucceeded;
        int serverNo; // which logical server - copy from request

        int socketId;
};

// Create a file system object
message spfsCreateRequest extends spfsRequest
{
    fields:
        int objectType;
};

// Create a file system object
message spfsCreateResponse extends spfsResponse
{
    fields:
        int handle;
};

// Remove a file system object
message spfsRemoveRequest extends spfsRequest
{
    fields:
};

// Remove a file system object
message spfsRemoveResponse extends spfsResponse
{
};

// Read object data
message spfsReadRequest extends spfsRequest
{
    fields:
        int serverNo;
        int serverCnt;
        unsigned long offset;
        int count;
        FSDataType dtype;
};

// Read file data
message spfsReadResponse extends spfsResponse
{
    fields:
        unsigned long bytes_read;
};

// Write object data
message spfsWriteRequest extends spfsRequest
{
    fields:
        int serverNo;
        int serverCnt;
        unsigned long offset;
        int count;
        FSDataType dtype;
};

// Write file data
message spfsWriteResponse extends spfsResponse
{
    fields:
        unsigned long bytesWritten;
};

// Write completion
message spfsWriteCompletionResponse extends spfsResponse
{
};

// Get file attributes
message spfsGetAttrRequest extends spfsRequest
{
    fields:
        string path; // remove this ???
};

// Get attributes
message spfsGetAttrResponse extends spfsResponse
{
    fields:
        FSMetaData meta;
};

// Set file attributes
message spfsSetAttrRequest extends spfsRequest
{
    fields:
        FSMetaData meta;
};

// Set attributes
message spfsSetAttrResponse extends spfsResponse
{
    fields:
        string attr[]; // remove this ???
};

// Resolve a filesystem path
message spfsLookupPathRequest extends spfsRequest
{
    fields:
        string path;
};

// Lookup a path
message spfsLookupPathResponse extends spfsResponse
{
    fields:
        int status enum(spfsLookupStatus);
        int handles[];
        int handleCount;
        int attrCount;
};

// Create a directory entry
message spfsCreateDirEntRequest extends spfsRequest
{
    fields:
        int parentHandle;
        int newHandle;
        string entry;
};

// Create a directory entry
message spfsCreateDirEntResponse extends spfsResponse
{
};

// Remove a directory entry
message spfsRemoveDirEntRequest extends spfsRequest
{
    fields:
        string entry;
};

// Remove a directory entry
message spfsRemoveDirEntResponse extends spfsResponse
{
    fields:
        int removedHandle;
};

// Resolve a filesystem path
message spfsChangeDirEntRequest extends spfsRequest
{
    fields:
        int parentHandle;
        int newHandle;
        string entry;
};

// Change a directory entry
message spfsChangeDirEntResponse extends spfsResponse
{
    fields:
        int oldDirEntHandle;
};

// Resolve a filesystem path
message spfsTruncateRequest extends spfsRequest
{
    fields:
        unsigned long newSize;
        int flags;
};

// Truncate a data object
message spfsTruncateResponse extends spfsResponse
{
};

// Resolve a filesystem path
message spfsMakeDirRequest extends spfsRequest
{
    fields:
        int handleExtents[];
};

// Create a directory
message spfsMakeDirResponse extends spfsResponse
{
    fields:
};

// Resolve a filesystem path
message spfsReadDirRequest extends spfsRequest
{
    fields:
        unsigned long dirOffset;
        unsigned long dirEntCount;
};

// Read a directory
message spfsReadDirResponse extends spfsResponse
{
    fields:
        unsigned long dirOffset;
        string entries[];
        int entryCount;
        unsigned long directoryVersion;
};

// Resolve a filesystem path
message spfsFlushRequest extends spfsRequest
{
    fields:
        string path;
};

// Flush a data object
message spfsFlushResponse extends spfsResponse
{
};

// Resolve a parallel filesystem path
message spfsStatRequest extends spfsRequest
{
    fields:
        int flags;
};

// Stat parallel filesystem
message spfsStatResponse extends spfsResponse
{
    fields:
        string stat;
};

// Resolve a parallel filesystem path
message spfsListAttrRequest extends spfsRequest
{
    fields:
        int handles[];
        int handleCount;
        int attrMask;
};

// List data object attributes
message spfsListAttrResponse extends spfsResponse
{
    fields:
        unsigned long numHandles;
};

//
// Local variables:
//  c-indent-level: 4
//  c-basic-offset: 4
// End:
//
// vim: ts=4 sts=4 sw=4 expandtab foldmethod=marker
//
