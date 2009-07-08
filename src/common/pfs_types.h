#ifndef PFS_TYPES_H
#define PFS_TYPES_H
//
// This file is part of Hecios
//
// Copyright (C) 2007,2008,2009 Brad Settlemyer
//
// This file is distributed WITHOUT ANY WARRANTY. See the file 'License.txt'
// for details on this and other legal matters.
//
#include <map>
#include <string>
#include <vector>
#include "basic_types.h"
class FileDistribution;

/** Possible status returns from a file system lookup */
enum FSLookupStatus { SPFS_INVALID_LOOKUP_STATUS = 0,
                      SPFS_FOUND = 1,
                      SPFS_PARTIAL = 2,
                      SPFS_NOTFOUND = 3, };

/** Parallel file system object types */
enum FSObjectType { SPFS_INVALID_OBJECT_TYPE = 0,
                    SPFS_DATA_OBJECT,
                    SPFS_DIR_ENT_OBJECT,
                    SPFS_DIRECTORY_OBJECT,
                    SPFS_METADATA_OBJECT, };

/** Metadata for a file */
struct FSMetaData
{
    /* standard Posix file metadata */
    int mode;
    int owner;
    int group;
    int nlinks;
    FSSize size;

    /** Metadata handle */
    FSHandle handle;

    /* Data object handles */
    std::vector<FSHandle> dataHandles;

    /** Data object sizes */
    std::vector<FSSize> bstreamSizes;

    /** File data distribution */
    FileDistribution* dist;
};

/** Equality operation for Metadata */
inline bool operator==(const FSMetaData& lhs, const FSMetaData& rhs)
{
    return (lhs.handle == rhs.handle);
}

/** Map for holding file system data for construction */
typedef std::map<std::string, FSSize> FileSystemMap;


#endif

/*
 * Local variables:
 *  indent-tabs-mode: nil
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 *
 * vim: ts=8 sts=4 sw=4 expandtab
 */
