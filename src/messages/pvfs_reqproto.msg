//
// Parallel File System request proto message definitions
//

// PVFS Request Protocol Messages
enum fsRequestTypes {
    FS_CREATE_REQUEST_KIND = 201;
    FS_REMOVE_REQUEST_KIND = 202;
    FS_READ_REQUEST_KIND = 203;
    FS_WRITE_REQUEST_KIND = 204;
    FS_GET_ATTR_REQEUST_KIND = 205;
    FS_SET_ATTR_REQUEST_KIND = 206;
    FS_LOOKUP_PATH_REQUEST_KIND = 207;
    FS_CREATE_DIR_ENT_REQUEST_KIND = 208;
    FS_REMOVE_DIR_ENT_REQUEST_KIND = 209;
    FS_CHANGE_DIR_ENT_REQUEST_KIND = 210;
    FS_TRUNCATE_REQUEST_KIND = 211;
    FS_MAKE_DIR_REQUEST_KIND = 212;
    FS_READ_DIR_REQUEST_KIND = 213;
    FS_FLUSH_REQUEST_KIND = 214;
    FS_STAT_REQUEST_KIND = 215;
    FS_LIST_ATTR_REQUEST_KIND = 216;
};

// File request abstract base class
message fsRequest
{
    fields:
        int fsId;
};

// Create a file system object
message fsCreateRequest extends fsRequest
{
    fields:
        int parent_handle;
        int handle_extent_array[];
        string entry;
};

// Remove a file system object
message fsRemoveRequest extends fsRequest
{
    fields:
        int handle;
};

// Read object data
message fsReadRequest extends fsRequest
{
    fields:
        int handle;
        int server_nr;
        int server_cnt;
        unsigned long offset;
        unsigned long aggregate_size;
};

// Write object data
message fsWriteRequest extends fsRequest
{
    fields:
        int handle;
        int server_nr;
        int server_cnt;
        unsigned long offset;
        unsigned long aggregate_size;
};

// Get file attributes
message fsGetAttrRequest extends fsRequest
{
    fields:
        string path;
};

// Set file attributes
message fsSetAttrRequest extends fsRequest
{
    fields:
        string path;
};

// Resolve a filesystem path
message fsLookupPathRequest extends fsRequest
{
    fields:
        string path;
};

// Create a directory entry
message fsCreateDirEntRequest extends fsRequest
{
    fields:
        int parent_handle;
        int new_handle;
        string entry;
};

// Remove a directory entry
message fsRemoveDirEntRequest extends fsRequest
{
    fields:
        int handle;
        string entry;
};

// Resolve a filesystem path
message fsChangeDirEntRequest extends fsRequest
{
    fields:
        int parent_handle;
        int new_handle;
        string entry;
};

// Resolve a filesystem path
message fsTruncateRequest extends fsRequest
{
    fields:
        int handle;
        unsigned long new_size;
        int flags;
};

// Resolve a filesystem path
message fsMakeDirRequest extends fsRequest
{
    fields:
        int handle_extents[];
};

// Resolve a filesystem path
message fsReadDirRequest extends fsRequest
{
    fields:
        int handle;
        unsigned long dir_offset;
        unsigned long dirent_count;
};

// Resolve a filesystem path
message fsFlushRequest extends fsRequest
{
    fields:
        string path;
};

// Resolve a filesystem path
message fsStatRequest extends fsRequest
{
    fields:
        int handle;
        int flags;
};

// Resolve a filesystem path
message fsListAttrRequest extends fsRequest
{
    fields:
        int handles[];
        int handle_count;
        int attr_mask;
};
