#ifndef PFS_TYPES_H
#define PFS_TYPES_H

#include <vector>
#include <string>
#include "basic_types.h"
class FileDistribution;

/** Maximum number of entries in a path */
#define MAXSEG 16

/** File system data distribution */
typedef int FSDataType;

/** Metadata for a file */
struct FSMetaData
{
    int mode;            /* standard Posix file metadata */
    int owner;
    int group;
    int nlinks;
    int size;            /* number of bytes in file */
    FSHandle handle; /* handle of the metadata object */
    std::vector<FSHandle> dataHandles;
    FileDistribution* dist;            
};

/** Equality operation for Metadata */
inline bool operator==(const FSMetaData& lhs, const FSMetaData& rhs)
{
    return (lhs.handle == rhs.handle);
}

/** Descriptor for an open file */
struct FSDescriptor
{
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

/** For backwards compatibility, alias FSDescriptor to FSOpenFile */
typedef struct FSDescriptor FSOpenFile;

#endif

/*
 * Local variables:
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 *
 * vim: ts=8 sts=4 sw=4 expandtab
 */
