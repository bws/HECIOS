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

protected:
    
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

    /** @return the blocks for an I/O request */
    virtual std::vector<FSBlock> getRequestBlocks(cMessage* msg) const = 0;
    
private:
    
    int inGateId_;

    int outGateId_;

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
    /** The default file system block size */
    static const std::size_t DEFAULT_BLOCK_SIZE_BYTES = 4096;
    
public:

    /** @return the file system's block size in bytes */
    std::size_t getFileBlockSize() const { return DEFAULT_BLOCK_SIZE_BYTES; };
    
protected:

    /**
     * @return a list of blocks that map to the corresponding file region
     */
    virtual std::vector<FSBlock> getRequestBlocks(cMessage* msg) const;

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
