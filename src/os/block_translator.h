#ifndef BLOCK_TRANSLATOR_H
#define BLOCK_TRANSLATOR_H
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

#include <omnetpp.h>
#include "basic_types.h"

/**
 * Abstract base class for the File System block translator.  The block
 * translator is responsible for translating the File System blocks (which
 * are not required to be sized similarly to disk geometry) into disk
 * hardware addresses (which for a hard drive will be sector numbers).
 */
class BlockTranslator : public cSimpleModule
{
public:
    /** Constructor */
    BlockTranslator();

protected:

    /** Initialize the mdule prior to simulation run */
    virtual void initialize();

    /** Perform any post simulation run cleanup */
    virtual void finish() {};

    /**
     *  This method is invoked by the simulation framework when a message
     *  is to be processed.
     *
     *  @param msg (in) is the message to be processed.
     */
    virtual void handleMessage(cMessage *msg);

    /**
     * @return a hardware address for the give file system block
     */
    virtual long long getAddress(FSBlock blocks) const = 0;

private:

    int inGateId_;
};

/**
 * Simply returns the existing block number as the disk address
 */
class NoTranslation : public BlockTranslator
{
public:
    /** Constructor */
    NoTranslation();

protected:

    /**
     * @return a hardware address for the give file system block
     */
    virtual long long getAddress(FSBlock block) const; 
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
