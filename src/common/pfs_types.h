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

#include <vector>
#include <string>
#include "basic_types.h"
class FileDistribution;

/** Maximum number of entries in a path */
#define MAXSEG 16

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
struct FSDescriptor1
{
    FSMetaData* metaData; /* pointer to file unique metadata */
    std::string path;    /* the complete path to the file */
    int filePtr;         /* offset of current position in file */
    
    FSHandle handles[MAXSEG]; /* handles of all dirs along path */    
    int segstart[MAXSEG];/* index to start of each dir name */
    int seglen[MAXSEG];  /* length to end of path for each segment */
    int curseg;          /* which segment is being looked up */
    int segcnt;
};

/** For backwards compatibility, alias FSDescriptor to FSOpenFile */
typedef struct FSDescriptor FSOpenFile1;

#endif

/*
 * Local variables:
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 *
 * vim: ts=8 sts=4 sw=4 expandtab
 */
