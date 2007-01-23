//
// Parallel File System request proto message definitions
//

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
