#ifndef FILE_SYSTEM_H
#define FILE_SYSTEM_H
//
// This file is part of Hecios
//
// Copyright (C) 2006 Joel Sherrill <joel@oarcorp.com>
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

#include <cstddef>
#include <vector>
#include <omnetpp.h>
#include "basic_types.h"
class Filename;
class StorageLayout;
class spfsOSFileIORequest;

/**
 * File System abstract base class module
 */
class FileSystem : public cSimpleModule
{
  public:

    /** Constructor */
    FileSystem();

    /** Destructor */
    virtual ~FileSystem() = 0;

    /** Add a directory to the file system */
    void createDirectory(const Filename& dirName);

    /** Add a file to the file system */
    void createFile(const Filename& filename, FSSize size);
    
protected:
    
    /** Initialize the simulation module */
    virtual void initialize();

    /** Finalize simulation module */
    virtual void finish();

    /** Handle simulation messages */
    virtual void handleMessage( cMessage *msg );

    /** Initialize derived file systems */
    virtual void initializeFileSystem() = 0;
    
    /** Finalize the native file system */
    virtual void finishFileSystem() = 0;

    /** Allocate disk storage for a new directory */
    virtual void allocateDirectoryStorage(const Filename& filename) = 0;

    /** Allocate disk storage for a new file */
    virtual void allocateFileStorage(const Filename& filename,
                                     FSSize size) = 0;
    
private:

    /** Send a request for the meta data blocks for a File I/O request */
    void sendMetaDataRequest(spfsOSFileIORequest* ioRequest);

    /** Send a request for the data blocks for a file I/O request */
    void sendDataRequest(spfsOSFileIORequest* ioRequest);
    
    /** @return the meta data blocks for a filename */
    virtual std::vector<FSBlock> getMetaDataBlocks(
        const Filename& filename) const = 0;
    
    /** @return the data blocks for a file region */
    virtual std::vector<FSBlock> getDataBlocks(
        const Filename& filename, FSOffset offset, FSSize extent) const = 0;
    
    /** in gate id */
    int inGateId_;

    /** out gate id */
    int outGateId_;

    /** request gate id */
    int requestGateId_;
};

/**
 * Model of a Native OS File System.  At present the only additional
 * functionality provided by this file system is the ability to convert
 * file locations into block numbers.  The block size is assumed to be 4096
 * bytes since that seems to match modern Linux systems.
 */
class NativeFileSystem : public FileSystem
{
public:
    
    /** @return the file system's block size in bytes */
    std::size_t getBlockSize() const { return blockSize_; };
    
protected:

    /** Initialize the native file system */
    virtual void initializeFileSystem();
    
    /** Finalize the native file system */
    virtual void finishFileSystem();
    
    /** Allocate disk storage for a new directory */
    virtual void allocateDirectoryStorage(const Filename& filename);

    /** Allocate disk storage for a new file */
    virtual void allocateFileStorage(const Filename& filename,
                                     FSSize size);
    
private:

    /** @return the meta data blocks for a filename */
    virtual std::vector<FSBlock> getMetaDataBlocks(
        const Filename& filename) const;
    
    /** @return the data blocks for a file region */
    virtual std::vector<FSBlock> getDataBlocks(
        const Filename& filename, FSOffset offset, FSSize extent) const;

    /** File system block size in bytes */
    std::size_t blockSize_;
    
    /** Storage layout for this file system */
    StorageLayout* storageLayout_;
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
