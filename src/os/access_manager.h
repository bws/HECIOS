#ifndef ACCESS_MANAGER_H
#define ACCESS_MANAGER_H
/**
 * @file access_manager.h
 * @brief Access Manager Modules
 *
 * Note: These classes are adapted from the FSS simulator project and are
 * licensed only under the GPL.  The FSS project is available at:
 * http://www.omnetpp.org/filemgmt/singlefile.php?lid=104 and
 * http://www.omnetpp.org/doc/FSS-doc/neddoc/index.html
 */

#include <omnetpp.h>

/**
 * Abstract base class for single-server queues
 */
class AbstractAccessManager : public cSimpleModule
{
public:
    /**
     *  This is the constructor for this simulation module.
     *
     *  @param namestr (in) is the name of the module
     *  @param parent (in) is the parent of this module
     *  @param stack (in) is the size in bytes of the stack for this module
     */
    AbstractAccessManager(const char *namestr=NULL, cModule *parent=NULL, size_t stack=0);

    /**
     *  This is the destructor for this simulation module.
     */
    ~AbstractAccessManager();

    /**
     *  This is the initialization routine for this simulation module.
     */
    virtual void initialize();

    /**
     *  This is the finalization routine for this simulation module.
     */
    virtual void finish();

    /**
     *  This is the message handling routine for this simulation module.
     *
     *  @param msg (in) is the new message to process
     */
    virtual void handleMessage(cMessage *msg);

protected:
    cMessage *RequestMsg;
    int fromSchedulerId;
    int fromDiskId;
    bool RequestPending;
    bool DiskIdle;

private:
    cMessage *msgServiced;
    cMessage *endServiceMsg;
    cQueue queue;    
};

/**
 * AccessManager that passes through all requests
 */
class SimpleAccessManager : public AbstractAccessManager
{
  public:
    /**
     *  This is the constructor for this simulation module.
     *
     *  @param namestr (in) is the name of the module
     *  @param parent (in) is the parent of this module
     *  @param stack (in) is the size in bytes of the stack for this module
     */
    SimpleAccessManager(const char *namestr=NULL, cModule *parent=NULL, size_t stack=0);
};

/**
 * AccessManager that allows only one outstanding request at a time
 */
class MutexAccessManager : public AbstractAccessManager
{
  public:
    /**
     *  This is the constructor for this simulation module.
     *
     *  @param namestr (in) is the name of the module
     *  @param parent (in) is the parent of this module
     *  @param stack (in) is the size in bytes of the stack for this module
     */
    MutexAccessManager(const char *namestr=NULL, cModule *parent=NULL, size_t stack=0);

    virtual void handleMessage( cMessage *msg );
};

#endif
