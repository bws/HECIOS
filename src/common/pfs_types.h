#ifndef PFS_TYPES_H
#define PFS_TYPES_H

#include <vector>

/** File system handle data type */
typedef long long FSHandle;

/** MPI Data type -- currently indicates number of bytes */
typedef int MPIDataType;

/** A contguous handle range beginning at first and ending at last */
struct HandleRange
{
    long first;
    long last;
};

/** Metadata for a file */
struct FSMetaData
{
    int mode;
    int owner;
    int group;
    int nlinks;
    int size;
    FSHandle metaHandle;
    std::vector<FSHandle> dataHandles;
    int dist;
};

/** Descriptor for an open file */
struct FSOpenFile
{
    FSHandle handle;
    int state;
    FSMetaData metaData;
    int filePtr;
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
