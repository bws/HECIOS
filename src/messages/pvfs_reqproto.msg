//
// Parallel File System request proto message definitions
//

// File request abstract base class
message fsRequest
{
    fields:
        int fsId;
};

// Create a directory
message fsCreateDirRequest extends fsRequest
{
    fields:
        int parent_handle;
        string entry;
};

// Create a file
message fsCreateFileRequest extends fsRequest
{
    fields:
        int parent_handle;
        int handle_extent_array[];
        string entry;
};

// Resolve a filesystem path
message fsLookupPathRequest extends fsRequest
{
    fields:
        string path;
};

// Get file attributes
message fsGetAttrRequest extends fsRequest
{
    fields:
        string path;
};

// Read file data
message fsReadFileRequest extends fsRequest
{
    fields:
        int handle;
};

// Remove a directory
message fsRemoveDirRequest extends fsRequest
{
    fields:
        int parentHandle;
        int targetHandle;
};

// Remove a file
message fsRemoveFileRequest extends fsRequest
{
    fields:
        int parentHandle;
        int targetHandle;
};

// Set file attributes
message fsSetAttrRequest extends fsRequest
{
    fields:
        string path;
};

// Write file data
message fsWriteFileRequest extends fsRequest
{
    fields:
        int handle;
        int server_nr;
        int server_cnt;
        unsigned long offset;
        unsigned long aggregate_size;
};
