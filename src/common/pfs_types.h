#ifndef PFS_TYPES_H
#define PFS_TYPES_H
//
// This file is part of Hecios
//
// Copyright (C) 2007 Brad Settlemyer
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
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
