#ifndef FILE_BUILDER_H
#define FILE_BUILDER_H
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
#include "pfs_types.h"
class Filename;
class StorageLayoutManager;

/** Builder functions for creating pre-existing parallel file system files */
class FileBuilder
{
public:

    /** Singleton accessor */
    static FileBuilder& instance();

    /** Clear all state information */
    static void clearState();
    
    /** @return the server id for the registering server */
    int registerFSServer(const HandleRange& range, bool isMetaServer);

    /** @return true if the filename exists within the PFS */
    bool fileExists(const Filename& fileName) const;

    /** @return a list of Metadata Server numbers */
    std::vector<int> getMetaServers() const;

    /** @return the number of data servers */
    size_t getNumDataServers() const {return nextServerNumber_;};
    
    /** @return Metadata for a PFS file */
    FSMetaData* getMetaData(const Filename& fileName) const;

    /** @return Descriptor for a PFS file */
    FSOpenFile* getDescriptor(const Filename& fileName) const;

    /** @return the first handle for the server */
    FSHandle getFirstHandle(size_t serverNumber);
    
    /** @return the next handle for server */
    FSHandle getNextHandle(size_t serverNumber);
    
    /** Create the named directory in the file system */
    void createDirectory(const Filename& dirName,
                         int metaServer,
                         StorageLayoutManager& layoutManager);
    
    /** Create the named file in the file system */
    void createFile(const Filename& fileName,
                    int metaServer,
                    int numDataServers,
                    StorageLayoutManager& layoutManager);

    /** @return the list of blocks for a file handle */
    std::vector<int> getDiskBlocks(const FSHandle& handle) const;
    
private:

    /** Default constructor */
    FileBuilder();
    
    /** Disabled copy constructor */
    FileBuilder(const FileBuilder& other);

    /** Singleton instance */
    static FileBuilder* instance_;

    std::map<std::string, FSHandle> nameToHandleMap_;

    std::map<FSHandle, FSMetaData*> handleToMetaMap_;

    size_t nextServerNumber_;

    std::vector<HandleRange> handlesByServer_;
    
    std::vector<int> metaServers_;

    std::vector<FSHandle> nextHandleByServer_;
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
