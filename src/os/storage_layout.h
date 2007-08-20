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

class StorageLayout
{
public:

    /** Constructor */
    StorageLayout();

    /** Destructor */
    ~StorageLayout();

    void addFile(FileSystem& fs, Filename& filename, FSSize size);
    
    vector<FSBlock> getFileSystemBlocks(const FileSystem& fs, FSOffset offset, FSSize extent);
    
private:

    /** Copy constructor */
    StorageLayout( StorageLayout& other );

    /** Assignment */
    StorageLayout operator=(StorageLayout& other );
    
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
