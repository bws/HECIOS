//
// Parallel File System request proto message definitions
//

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

