//
// Parallel File System request proto message definitions
//

// File system response abstract base class
message fsResponse
{
    fields:
        bool requestSucceeded;
};

// Create a directory
message fsCreateDirResponse extends fsResponse
{
    fields:
        int handle;
};

// Create a file
message fsCreateFileResponse extends fsResponse
{
    fields:
        int handle;
};

// Lookup a path
message fsLookupPathResponse extends fsResponse
{
    fields:
        int handles[];
        int handle_count;
        int attr_count;
};

// Get attributes
message fsGetAttrResponse extends fsResponse
{
};

// Set attributes
message fsSetAttrResponse extends fsResponse
{
};

// Read file data
message fsReadFileResponse extends fsResponse
{
};

// Remove a directory
message fsRemoveDirResponse extends fsResponse
{
};

// Remove a file
message fsRemoveFileResponse extends fsResponse
{
};

// Write file data
message fsWriteFileResponse extends fsResponse
{
    fields:
        unsigned long data_written;
};

