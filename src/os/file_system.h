#ifndef FILE_SYSTEM_H
#define FILE_SYSTEM_H
//
// This file is part of Hecios
//
// Copyright (C) 2006 Joel Sherrill <joel@oarcorp.com>
// Copyright (C) 2007,2008,2009 Brad Settlemyer
//
// This file is distributed WITHOUT ANY WARRANTY. See the file 'License.txt'
// for details on this and other legal matters.
//
#include <cstddef>
#include <vector>
#include <omnetpp.h>
#include "basic_types.h"
class Filename;
class StorageLayout;
class spfsOSFileLIORequest;
class spfsOSFileOpenRequest;
class spfsOSFileUnlinkRequest;
class spfsOSFileRequest;

/**
 * File System abstract base class module.  Supports the following standard
 * file system features:
 *
 * - Writes meta data on file cretion
 * - Reads meta data on file open
 * - Loads metadata before accessing data blocks
 * - Writes first metadata block on reads and writes (i.e. modifies the atime)
 *
 * TODO: The following additional feature(s) are desired:
 * - Read block before writing partial blocks
 * - Journal structures for metadata writing
 * - Support for an O_DIRECT type mode that bypasses the block cache
 *
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
    /** Process the the multiple messages for a single File request */
    void processMessage(spfsOSFileRequest* request, cMessage* msg);

    /** Process the the multiple messages for a single File open request */
    void processOpenMessage(spfsOSFileOpenRequest* request, cMessage* msg);

    /** Process the the multiple messages for a single File unlink request */
    void processUnlinkMessage(spfsOSFileUnlinkRequest* request, cMessage* msg);

    /** Process the the multiple messages for a single File I/O request */
    void processIOMessage(spfsOSFileLIORequest* request, cMessage* msg);

    /** Send a request for the meta data blocks for a File I/O request */
    void readMetaData(spfsOSFileRequest* request);

    /** Update the meta data to signal the file I/O has been performed */
    void writeMetaData(spfsOSFileRequest* request);

    /** Send a request for the data blocks for a file I/O request */
    void performIO(spfsOSFileLIORequest* ioRequest);

    /** Send the final read or write response */
    void sendFileIOResponse(spfsOSFileLIORequest* ioRequest);

    /** @return the meta data blocks for a filename */
    virtual std::vector<FSBlock> getMetaDataBlocks(
        const Filename& filename) const = 0;

    /** @return the data blocks for a file region */
    virtual std::vector<FSBlock> getDataBlocks(
        spfsOSFileLIORequest* ioRequest) const = 0;

    /** Flag indicating if atime is updated on each access */
    bool noATime_;

    /** in gate id */
    int inGateId_;

    /** out gate id */
    int outGateId_;

    /** request gate id */
    int requestGateId_;
};

/**
 * Model of a Native OS File System.  Supports the following features:
 *
 * - Configurable block size
 * - Contiguous disk layout
 *
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

    /** @return the data blocks for a list file region */
    virtual std::vector<FSBlock> getDataBlocks(
        spfsOSFileLIORequest* ioRequest) const;

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
