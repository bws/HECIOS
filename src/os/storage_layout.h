#ifndef STORAGE_LAYOUT_H
#define STORAGE_LAYOUT_H
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
#include <vector>
#include "basic_types.h"
#include "filename.h"

/** Provides a contiguous disk layout */
class StorageLayout
{
public:

    /** The number of data blocks to assign to a directory */
    static const std::size_t NUM_DIRECTORY_DATA_BLOCKS = 10;
    
    /** Constructor */
    StorageLayout(std::size_t blockSize);

    /** Destructor */
    ~StorageLayout() {};

    /** Add layout information for a directory */
    void addDirectory(const Filename& filename);

    /** Add layout information for a file */
    void addFile(const Filename& filename, FSSize size);

    /** @return vector of data blocks for a file's offset and extent */
    std::vector<FSBlock> getFileDataBlocks(const Filename& file,
                                                 FSOffset offset,
                                                 FSSize extent) const;

    /** @return the metadata blocks associated with a file */
    std::vector<FSBlock> getFileMetaDataBlocks(
        const Filename& file) const;
    
private:

    /** Copy constructor */
    StorageLayout( StorageLayout& other );

    /** Assignment */
    StorageLayout operator=(StorageLayout& other );

    /** File system's block size */
    std::size_t fsBlockSize_;
    
    /** Next block to use for meta data */
    FSBlock nextMetaDataBlock_;
    
    /** Next block to use for file data */
    FSBlock nextDataBlock_;
    
    /** Map to the first inode block for a file */
    std::map<Filename, FSBlock> metaDataBlocks_;

    /** Map to the first data block for a file */
    std::map<Filename, FSBlock> dataBlocks_;
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
