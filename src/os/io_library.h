#ifndef IO_LIBRARY_H
#define IO_LIBRARY_H
/**
 * @file iolibrary.h
 * @brief IO Library Modules
 *
 * Note: These classes are adapted from the FSS simulator project and are
 * licensed only under the GPL.  The FSS project is available at:
 * http://www.omnetpp.org/filemgmt/singlefile.php?lid=104 and
 * http://www.omnetpp.org/doc/FSS-doc/neddoc/index.html
 */

#include <omnetpp.h>
class NativeFileSystem;

/**
 * Abstract base class for I/O library models
 */
class AbstractIOLibrary : public cSimpleModule
{
  public:

    cQueue queue;

    int fromInGateId;

    /**
     *  This is the initialization routine for this simulation module.
     */
    virtual void initialize();

    /**
     *  This is the finalization routine for this simulation module.
     */
    virtual void handleMessage(cMessage *msg);

    // hook functions to (re)define behaviour
};

/**
 * I/O library model that passes requests through with no translation
 */
class PassThroughIOLibrary : public AbstractIOLibrary
{
};

/**
 *
 */
class PFSIOLibrary : public AbstractIOLibrary
{
public:

    /** */
    virtual void initialize();
    
    /** Translate messages from FSS style to hecios style messages */
    virtual void handleMessage(cMessage *msg);

private:

    /** in gate connection */
    int inGateId_;

    /** out gate connection */
    int outGateId_;

    /** request gate connection */
    int requestGateId_;

    /** Disk model */
    NativeFileSystem* localFileSystem_;
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
