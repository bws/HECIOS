//
// Parallel File System request proto message definitions
//

// PVFS Response Protocol Messages
enum fsResponseTypes {
    FS_CREATE_RESPONSE_KIND = 301;
    FS_REMOVE_RESPONSE_KIND = 302;
    FS_READ_RESPONSE_KIND = 303;
    FS_WRITE_RESPONSE_KIND = 304;
    FS_GET_ATTR_REQEUST_KIND = 305;
    FS_SET_ATTR_RESPONSE_KIND = 306;
    FS_LOOKUP_PATH_RESPONSE_KIND = 307;
    FS_CREATE_DIR_ENT_RESPONSE_KIND = 308;
    FS_REMOVE_DIR_ENT_RESPONSE_KIND = 309;
    FS_CHANGE_DIR_ENT_RESPONSE_KIND = 310;
    FS_TRUNCATE_RESPONSE_KIND = 311;
    FS_MAKE_DIR_RESPONSE_KIND = 312;
    FS_READ_DIR_RESPONSE_KIND = 313;
    FS_WRITE_COMPLETION_RESPONSE_KIND = 314;
    FS_FLUSH_RESPONSE_KIND = 315;
    FS_STAT_RESPONSE_KIND = 316;
    FS_LIST_ATTR_RESPONSE_KIND = 317;
}

// File system response abstract base class
message fsResponse
{
    fields:
        bool requestSucceeded;
};

// Create a file system object
message fsCreateResponse extends fsResponse
{
    fields:
        int handle;
};

// Remove a file system object
message fsRemoveResponse extends fsResponse
{
};

// Read file data
message fsReadResponse extends fsResponse
{
    fields:
        unsigned long bstream_size;
};

// Write file data
message fsWriteResponse extends fsResponse
{
    fields:
        unsigned long bstream_size;
};

// Get attributes
message fsGetAttrResponse extends fsResponse
{
    fields:
        string attr;
};

// Set attributes
message fsSetAttrResponse extends fsResponse
{
    fields:
        string attr[];
};

// Lookup a path
message fsLookupPathResponse extends fsResponse
{
    fields:
        int handles[];
        int handle_count;
        int attr_count;
};

// Create a directory entry
message fsCreateDirEntResponse extends fsResponse
{
};

// Remove a directory entry
message fsRemoveDirEntResponse extends fsResponse
{
    fields:
        int removedHandle;
};

// Change a directory entry
message fsChangeDirEntResponse extends fsResponse
{
    fields:
        int old_dirent_handle;
};

// Truncate a data object
message fsTruncateResponse extends fsResponse
{
};

// Create a directory
message fsMakeDirResponse extends fsResponse
{
    fields:
        int handle;
};

// Read a directory
message fsReadDirResponse extends fsResponse
{
    fields:
        int handle;
        unsigned long dir_offset;
        string entries[];
        int entry_count;
        unsigned long directory_version;
};

// Write completion
message fsWriteCompletionResponse extends fsResponse
{
};

// Flush a data object
message fsFlushResponse extends fsResponse
{
};

// Stat filesystem
message fsStatResponse extends fsResponse
{
    fields:
        string stat;
};

// List data object attributes
message fsListAttrResponse extends fsResponse
{
    fields:
        unsigned long num_handles;
};

