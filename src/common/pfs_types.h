#ifndef PFS_TYPES_H
#define PFS_TYPES_H

#include <vector>
#include <string>
class ClientFSState;

#define MAXSEG 16  /* maximum number of entries in a path */

/** File system handle data type */
typedef long long FSHandle;

/** MPI Data type -- currently indicates number of bytes */
typedef int MPIDataType;

/** A contguous handle range beginning at first and ending at last */
struct HandleRange
{
    FSHandle first;
    FSHandle last;
};

enum MPI_Modes
{
    MPI_MODE_RDONLY = 1,
    MPI_MODE_WRONLY = 2,
    MPI_MODE_RDWR = 4,
    MPI_MODE_CREATE = 8,
    MPI_MODE_EXCL = 16,
};


/** Metadata for a file */
struct FSMetaData
{
    int mode;            /* standard Posix file metadata */
    int owner;
    int group;
    int nlinks;
    int size;            /* number of bytes in file */
    FSHandle metaHandle; /* handle of the metadata object */
    std::vector<FSHandle> dataHandles; /* size of handles is server count */
    int dist;            /* for now just strip size in bytes */
};

/** Descriptor for an open file */
struct FSOpenFile
{
    ClientFSState *fs;   /* pointer to client unique fs struct */
    FSHandle handle;     /* handle of the file - the metadata object */
    int state;           /* I don't think we need this - WBL */
    FSMetaData metaData; /* pointer to file unique metadata */
    std::string path;    /* the complete path to the file */
    int filePtr;         /* offset of current position in file */
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
