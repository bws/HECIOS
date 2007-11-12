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

/** An abstract storage layout interface */
class StorageLayout
{
public:

    /** Constructor */
    StorageLayout();

    /** Destructor */
    virtual ~StorageLayout() = 0;

    /** Add layout information for a directory */
    void addDirectory(const Filename& filename);

    /** Add layout information for a file */
    void addFile(const Filename& filename, FSSize size);

    /** @return vector of data blocks for a file's offset and extent */
    std::vector<FSBlock> getFileDataBlocks(const Filename& file,
                                           FSOffset offset,
                                           FSSize extent) const;

    /** @return vector of data blocks for a file and vector of file regions */
    std::vector<FSBlock> getFileDataBlocks(
        const Filename& file, std::vector<FileRegion> regions) const;

    /** @return the metadata blocks associated with a file */
    std::vector<FSBlock> getFileMetaDataBlocks(
        const Filename& file) const;

protected:
    /** Add layout information for a directory */
    virtual void addDirectoryToLayout(const Filename& filename) = 0;

    /** Add layout information for a file */
    virtual void addFileToLayout(const Filename& filename, FSSize size) = 0;

    /** @return vector of data blocks for a file and vector of file regions */
    virtual std::vector<FSBlock> getLayoutFileDataBlocks(
        const Filename& file, std::vector<FileRegion> regions) const = 0;

    /** @return the metadata blocks associated with a file */
    virtual std::vector<FSBlock> getLayoutFileMetaDataBlocks(
        const Filename& file) const = 0;
    
private:
    /** Copy constructor */
    StorageLayout( StorageLayout& other );

    /** Assignment */
    StorageLayout& operator=(StorageLayout& other );
};

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
