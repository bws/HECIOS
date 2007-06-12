#ifndef PFS_TYPES_H
#define PFS_TYPES_H

#include <vector>
#include <string>
class ClientFSState;

/** Maximum number of entries in a path */
#define MAXSEG 16

/** File system data distribution */
typedef int FSDataType;

/** File system handle data type */
typedef long long FSHandle;

/** A contguous handle range beginning at first and ending at last */
struct HandleRange
{
    FSHandle first;
    FSHandle last;
};

/** Equality operation for HandleRanges */
inline bool operator==(const HandleRange& lhs, const HandleRange& rhs)
{
    return (lhs.first == rhs.first && lhs.last == rhs.last);
}

/**
 * Less than operation for HandleRanges
 *
 * Only provided so that HandleRanges can be inserted into maps,
 * Note:  Assumes that HandleRanges do not overlap!!
 */
inline bool operator<(const HandleRange& lhs, const HandleRange& rhs)
{
    return (lhs.first < rhs.first && lhs.last < rhs.last);
}

/** Metadata for a file */
struct FSMetaData
{
    int mode;            /* standard Posix file metadata */
    int owner;
    int group;
    int nlinks;
    int size;            /* number of bytes in file */
    FSHandle handle; /* handle of the metadata object */
    std::vector<FSHandle> dataHandles; /* size of handles is server count */
    int dist;            /* for now just strip size in bytes */
};

/** Equality operation for Metadata */
inline bool operator==(const FSMetaData& lhs, const FSMetaData& rhs)
{
    return (lhs.handle == rhs.handle);
}

/** Descriptor for an open file */
struct FSOpenFile
{
    //ClientFSState *fs;   /* pointer to client unique fs struct */
    FSMetaData* metaData; /* pointer to file unique metadata */
    std::string path;    /* the complete path to the file */
    int filePtr;         /* offset of current position in file */
    
    FSHandle handle; // Is this field really neccesary

    FSHandle handles[MAXSEG]; /* handles of all dirs along path */    
    int segstart[MAXSEG];/* index to start of each dir name */
    int seglen[MAXSEG];  /* length to end of path for each segment */
    int curseg;          /* which segment is being looked up */
    int segcnt;
};

#endif

/*
 * Local variables:
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 *
 * vim: ts=8 sts=4 sw=4 expandtab
 */
