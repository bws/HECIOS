/**
 * @file blocktrans.h
 * @brief Block Translator Modules
 */

/*
 *  $Id: block_translator.h,v 1.1 2007/04/05 22:57:17 bradles Exp $
 */

#ifndef __BLOCKTRANS_H
#define __BLOCKTRANS_H

#include <omnetpp.h>

/**
 * Abstract base class for the Disk Block Number translation layer
 * in the filesystem software stack.  The filesystem views the disk
 * as an ordered array of disk blocks.  This layer is responsible
 * for translating that abstract view into head, cylinder, and sector
 * as required by the disk.
 */
class AbstractBlockTranslator : public cSimpleModule
{
    cMessage *msgServiced;
    cMessage *endServiceMsg;
    cQueue queue;

  public:
    /**
     *  This is the constructor for this simulation module.
     *
     *  @param namestr (in) is the name of the module
     *  @param parent (in) is the parent of this module
     *  @param stack (in) is the size in bytes of the stack for this module
     */
    AbstractBlockTranslator(
     const char *namestr,
     cModule *parent,
     size_t stack=0
    );

    /**
     *  This method is invoked by the simulation framework when a message
     *  is to be processed.
     *
     *  @param msg (in) is the message to be processed.
     */
    virtual void handleMessage(cMessage *msg);

    /**
     *  This method is invoked when each request arrives.  It translates
     *  the logical block number into the information required by the
     *  disk hardware.
     *
     *  @param msg (in) is the message to be processed.
     *
     *  @note This method is a hook function which is assumed to be
     *  provided by a derived class to (re)define the behaviour.
     */
    virtual void Translate(cMessage *msg) {}
};

/**
 * No translation: Simply pass through the request
 */
class NoTranslation : public AbstractBlockTranslator
{
  public:
    /**
     *  This is the constructor for this simulation module.
     *
     *  @param namestr (in) is the name of the module
     *  @param parent (in) is the parent of this module
     *  @param stack (in) is the size in bytes of the stack for this module
     */
    NoTranslation(const char *namestr=NULL, cModule *parent=NULL, size_t stack=0);

    /**
     *  @copydoc AbstractBlockTranslator::Translate
     *
     *  @par Derived Implementation Details:
     *
     *  This particular implementation performs no translation
     *  and assumes that the logical block number is the
     *  same as that used by the disk hardware.  In other words, the
     *  interface of the physical disk hardware presents itself as an
     *  array of disk blocks.
     */
    virtual void Translate(cMessage *msg);
};

#endif
