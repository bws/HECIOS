#ifndef SYSTEM_CALL_INTERFACE_H
#define SYSTEM_CALL_INTERFACE_H
/**
 * @file system_call_interface.h
 * @brief System Call Modules
 *
 * Note: These classes are adapted from the FSS simulator project and are
 * licensed only under the GPL.  The FSS project is available at:
 * http://www.omnetpp.org/filemgmt/singlefile.php?lid=104 and
 * http://www.omnetpp.org/doc/FSS-doc/neddoc/index.html
*/

#include <omnetpp.h>

/**
 * Abstract base class for system call interface models
 */
class AbstractSystemCallInterface : public cSimpleModule
{
  public:
    /**
     *  This is the constructor for this simulation module.
     *
     *  @param namestr (in) is the name of the module
     *  @param parent (in) is the parent of this module
     *  @param stack (in) is the size in bytes of the stack for this module
     */
    AbstractSystemCallInterface(
      const char *namestr=NULL, cModule *parent=NULL, size_t stack=0);

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
};

/**
 * simple File System model that passes requests through with no translation
 */
class PassThroughSystemCallInterface : public AbstractSystemCallInterface
{
  public:
    /**
     *  This is the constructor for this simulation module.
     *
     *  @param namestr (in) is the name of the module
     *  @param parent (in) is the parent of this module
     *  @param stack (in) is the size in bytes of the stack for this module
     */
    PassThroughSystemCallInterface(
      const char *namestr=NULL, cModule *parent=NULL, size_t stack=0);
};

#endif

