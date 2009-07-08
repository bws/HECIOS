#ifndef BLOCK_TRANSLATOR_H
#define BLOCK_TRANSLATOR_H
//
// This file is part of Hecios
//
// Copyright (C) 2007,2008,2009 Brad Settlemyer
//
// This file is distributed WITHOUT ANY WARRANTY. See the file 'License.txt'
// for details on this and other legal matters.
//
#include <vector>
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
     * Initialize derived translator functionality
     */
    virtual void initializeTranslator() = 0;

    /**
     * @return a hardware address for the give file system block
     */
    virtual std::vector<LogicalBlockAddress> getAddresses(
        FSBlock blocks) const = 0;

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

    /** Initialize the translator (no-op) */
    virtual void initializeTranslator() {};

    /**
     * @return a hardware address for the give file system block
     */
    virtual std::vector<LogicalBlockAddress> getAddresses(
        FSBlock block) const;
};

/**
 * Translate 4K filesystem blocks into 512 byte disk blocks.  Uses an
 * identical disk and filesystem fragmentation.
 */
class BasicTranslator : public BlockTranslator
{
public:
    /** Constructor */
    BasicTranslator();

protected:

    /** */
    virtual void initializeTranslator();

    /**
     * @return a hardware address for the give file system block
     */
    virtual std::vector<LogicalBlockAddress> getAddresses(
        FSBlock block) const;

private:

    int addrsPerBlock_;
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
