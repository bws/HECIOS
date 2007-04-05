//
// Parallel File System request proto message definitions
//

cplusplus {{
#include <pfs_types.h>
}}

struct FSMetaData;
struct HandleRange;
class noncobject MPIDataType; // this is an int

// PVFS Request Protocol Messages
enum fsRequestTypes {
    FS_CREATE_REQUEST = 201;
    FS_REMOVE_REQUEST = 202;
    FS_READ_REQUEST = 203;
    FS_WRITE_REQUEST = 204;
    FS_GET_ATTR_REQUEST = 205;
    FS_SET_ATTR_REQUEST = 206;
    FS_LOOKUP_PATH_REQUEST = 207;
    FS_CREATE_DIR_ENT_REQUEST = 208;
    FS_REMOVE_DIR_ENT_REQUEST = 209;
    FS_CHANGE_DIR_ENT_REQUEST = 210;
    FS_TRUNCATE_REQUEST = 211;
    FS_MAKE_DIR_REQUEST = 212;
    FS_READ_DIR_REQUEST = 213;
    FS_FLUSH_REQUEST = 214;
    FS_STAT_REQUEST = 215;
    FS_LIST_ATTR_REQUEST = 216;
};

// PVFS Response Protocol Messages
enum fsResponseTypes {
    FS_CREATE_RESPONSE = 301;
    FS_REMOVE_RESPONSE = 302;
    FS_READ_RESPONSE = 303;
    FS_WRITE_RESPONSE = 304;
    FS_GET_ATTR_RESPONSE = 305;
    FS_SET_ATTR_RESPONSE = 306;
    FS_LOOKUP_PATH_RESPONSE = 307;
    FS_CREATE_DIR_ENT_RESPONSE = 308;
    FS_REMOVE_DIR_ENT_RESPONSE = 309;
    FS_CHANGE_DIR_ENT_RESPONSE = 310;
    FS_TRUNCATE_RESPONSE = 311;
    FS_MAKE_DIR_RESPONSE = 312;
    FS_READ_DIR_RESPONSE = 313;
    FS_WRITE_COMPLETION_RESPONSE = 314;
    FS_FLUSH_RESPONSE = 315;
    FS_STAT_RESPONSE = 316;
    FS_LIST_ATTR_RESPONSE = 317;
}

enum fsLookupStatus {
    FS_FOUND = 1;
	 FS_PARTIAL = 2;
	 FS_NOTFOUND = 3;
}


// File request abstract base class
message fsRequest
{
    fields:
        int fsId;
		  int serverNo; // which logical server
		  int handle;   // indicates server address and object
};

// File system response abstract base class
message fsResponse
{
    fields:
        bool requestSucceeded;
		  int serverNo; // which logical server - copy from request
};

// Create a file system object
message fsCreateRequest extends fsRequest
{
    fields:
        int objectType;
        HandleRange handleRng;
};

// Create a file system object
message fsCreateResponse extends fsResponse
{
    fields:
        int handle;
};

// Remove a file system object
message fsRemoveRequest extends fsRequest
{
    fields:
};

// Remove a file system object
message fsRemoveResponse extends fsResponse
{
};

// Read object data
message fsReadRequest extends fsRequest
{
    fields:
        int serverNo;
        int serverCnt;
        unsigned long offset;
		  int count;
	     MPIDataType dtype;
};

// Read file data
message fsReadResponse extends fsResponse
{
    fields:
        unsigned long bytes_read;
};

// Write object data
message fsWriteRequest extends fsRequest
{
    fields:
        int server_nr;
        int server_cnt;
        unsigned long offset;
	     int count;
	     MPIDataType dtype;
};

// Write file data
message fsWriteResponse extends fsResponse
{
    fields:
        unsigned long bytes_written;
};

// Write completion
message fsWriteCompletionResponse extends fsResponse
{
};

// Get file attributes
message fsGetAttrRequest extends fsRequest
{
    fields:
        string path; // remove this ???
};

// Get attributes
message fsGetAttrResponse extends fsResponse
{
    fields:
        FSMetaData meta;
};

// Set file attributes
message fsSetAttrRequest extends fsRequest
{
    fields:
        FSMetaData meta;
};

// Set attributes
message fsSetAttrResponse extends fsResponse
{
    fields:
        string attr[]; // remove this ???
};

// Resolve a filesystem path
message fsLookupPathRequest extends fsRequest
{
    fields:
        string path;
};

// Lookup a path
message fsLookupPathResponse extends fsResponse
{
    fields:
	     int status enum(fsLookupStatus);
        int handles[];
        int handle_count;
        int attr_count;
};

// Create a directory entry
message fsCreateDirEntRequest extends fsRequest
{
    fields:
        int parent_handle;
        int new_handle;
        string entry;
};

// Create a directory entry
message fsCreateDirEntResponse extends fsResponse
{
};

// Remove a directory entry
message fsRemoveDirEntRequest extends fsRequest
{
    fields:
        string entry;
};

// Remove a directory entry
message fsRemoveDirEntResponse extends fsResponse
{
    fields:
        int removedHandle;
};

// Resolve a filesystem path
message fsChangeDirEntRequest extends fsRequest
{
    fields:
        int parent_handle;
        int new_handle;
        string entry;
};

// Change a directory entry
message fsChangeDirEntResponse extends fsResponse
{
    fields:
        int old_dirent_handle;
};

// Resolve a filesystem path
message fsTruncateRequest extends fsRequest
{
    fields:
        unsigned long new_size;
        int flags;
};

// Truncate a data object
message fsTruncateResponse extends fsResponse
{
};

// Resolve a filesystem path
message fsMakeDirRequest extends fsRequest
{
    fields:
        int handle_extents[];
};

// Create a directory
message fsMakeDirResponse extends fsResponse
{
    fields:
};

// Resolve a filesystem path
message fsReadDirRequest extends fsRequest
{
    fields:
        unsigned long dir_offset;
        unsigned long dirent_count;
};

// Read a directory
message fsReadDirResponse extends fsResponse
{
    fields:
        unsigned long dir_offset;
        string entries[];
        int entry_count;
        unsigned long directory_version;
};

// Resolve a filesystem path
message fsFlushRequest extends fsRequest
{
    fields:
        string path;
};

// Flush a data object
message fsFlushResponse extends fsResponse
{
};

// Resolve a filesystem path
message fsStatRequest extends fsRequest
{
    fields:
        int flags;
};

// Stat filesystem
message fsStatResponse extends fsResponse
{
    fields:
        string stat;
};

// Resolve a filesystem path
message fsListAttrRequest extends fsRequest
{
    fields:
        int handles[];
        int handle_count;
        int attr_mask;
};

// List data object attributes
message fsListAttrResponse extends fsResponse
{
    fields:
        unsigned long num_handles;
};
