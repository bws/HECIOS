#ifndef FILE_SYSTEM_H
#define FILE_SYSTEM_H
/**
 * @file file_system.h
 * @brief File System Modules
 *
 * Note: These classes are adapted from the FSS simulator project and are
 * licensed only under the GPL.  The FSS project is available at:
 * http://www.omnetpp.org/filemgmt/singlefile.php?lid=104 and
 * http://www.omnetpp.org/doc/FSS-doc/neddoc/index.html
 */

#include <vector>
#include <omnetpp.h>
#include "pfs_types.h"

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
    
    /**
     *  This is the initialization routine for this simulation module.
     */
    virtual void initialize();

    /**
     *  This is the finalization routine for this simulation module.
     */
    virtual void finish();

    /**
     *  This is the finalization routine for this simulation module.
     */
    virtual void handleMessage( cMessage *msg );

    cQueue queue;

    int fromInGateId;

};

/**
 * Model of a Native OS File System.  At present the only additional
 * functionality provided by this file system is the ability to convert
 * file locations into block numbers.  The block size is assumed to be 512
 * bytes since that seems to match our disk models most closely.
 */
class NativeFileSystem : public FileSystem
{
    /** The default file system block size */
    static const std::size_t DEFAULT_BLOCK_SIZE_BYTES = 4096;
    
public:

    /** @return the file system's block size in bytes */
    std::size_t getFileBlockSize() const { return DEFAULT_BLOCK_SIZE_BYTES; };
    
    /**
     * @return a list of blocks that map to the corresponding file region
     */
    std::vector<long> getBlocks(FSHandle handle, size_t offset, size_t bytes);
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
