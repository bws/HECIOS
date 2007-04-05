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

/**
 * Abstract base class for I/O library models
 */
class AbstractIOLibrary : public cSimpleModule
{
  public:
    /**
     *  This is the constructor for this simulation module.
     *
     *  @param namestr (in) is the name of the module
     *  @param parent (in) is the parent of this module
     *  @param stack (in) is the size in bytes of the stack for this module
     */
    AbstractIOLibrary(const char *namestr=NULL, cModule *parent=NULL, size_t stack=0);

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
  public:
    /**
     *  This is the constructor for this simulation module.
     *
     *  @param namestr (in) is the name of the module
     *  @param parent (in) is the parent of this module
     *  @param stack (in) is the size in bytes of the stack for this module
     */
    PassThroughIOLibrary(const char *namestr=NULL, cModule *parent=NULL, size_t stack=0);
};

#endif
