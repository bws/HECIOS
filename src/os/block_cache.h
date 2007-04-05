#ifndef BLOCK_CACHE_H
#define BLOCK_CACHE_H
/**
 * @file block_cache.h
 * @brief Cache Manager Modules
 *
 * Note: These classes are adapted from the FSS simulator project and are
 * licensed only under the GPL.  The FSS project is available at:
 * http://www.omnetpp.org/filemgmt/singlefile.php?lid=104 and
 * http://www.omnetpp.org/doc/FSS-doc/neddoc/index.html
*/

#include "priority_distribution.h"
#include <omnetpp.h>

/**
 * Abstract base class for Cache Managers
 */
class AbstractCache : public cSimpleModule
{
    long number_of_inversions;
    long number_of_requests;
    long number_of_hits;
    long number_of_misses;

  protected:
    long number_of_elements;
    cQueue Cache;

  public:
    /**
     *  This is the constructor for this simulation module.
     *
     *  @param namestr (in) is the name of the module
     *  @param parent (in) is the parent of this module
     *  @param stack (in) is the size in bytes of the stack for this module
     */
    AbstractCache(const char *namestr=NULL, cModule *parent=NULL, size_t stack=0);

    /**
     *  This is the initialization routine for this simulation module.
     */
    virtual void initialize();

    /**
     *  This method is invoked by the simulation framework when a message
     *  is to be processed.
     *
     *  @param msg (in) is the message to be processed.
     */
    virtual void handleMessage(cMessage *msg);

    /**
     *  This is the finalization routine for this simulation module.
     */
    virtual void finish();

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
    virtual void setup(void) {};

    /**
     *  This method is invoked at the beginning of the simulation
     *  run by the @ref AbstractCache::initialize method to allow for
     *  Cache algorithm specific initialization actions.
     *
     *  @param msg (in) is the message to be processed.
     *
     *  @note This method is a hook function which is assumed to be
     *  provided by a derived class to (re)define the behaviour.
     */
    virtual void initializeCache(void);

    /**
     *  This method is invoked when a block is added to the cached set.
     *  It determines if a priority inversion is in process and maintains
     *  statistics about them.
     *
     *  This implementation is intended to be sufficient for checking
     *  for priority investions with all Disk Caching algorithms.
     *
     *  @param msg (in) is the message to be processed.
     *
     */
    void checkForPriorityInversion(cMessage *msg);

    /**
     *  This method is invoked when each request arrives.  It checks
     *  if the requested block is cached.  If it is cached, then this
     *  method returns TRUE.  Otherwise, this method is responsible for
     *  selecting a method to eject from the cache.  If a new block
     *  is cached, it is the responsibility of this method to invoke
     *  @ref checkForPriorityInversion.
     *
     *  @param msg (in) is the message to be processed.
     *
     *  @note This method is a hook function which is assumed to be
     *  provided by a derived class to (re)define the behaviour.
     */
    virtual bool checkCache(cMessage *msg);
};

/**
 * No cache: all requests are passed through to the Disk Subsystem
 */
class NoCache : public AbstractCache
{
  public:
    /**
     *  This is the constructor for this simulation module.
     *
     *  @param namestr (in) is the name of the module
     *  @param parent (in) is the parent of this module
     *  @param stack (in) is the size in bytes of the stack for this module
     */
    NoCache(const char *namestr=NULL, cModule *parent=NULL, size_t stack=0);
};

/**
 * FIFO Cache Manager
 */
class FIFOCache : public AbstractCache
{
  public:
    /**
     *  This is the constructor for this simulation module.
     *
     *  @param namestr (in) is the name of the module
     *  @param parent (in) is the parent of this module
     *  @param stack (in) is the size in bytes of the stack for this module
     */
    FIFOCache(const char *namestr=NULL, cModule *parent=NULL, size_t stack=0);

    /**
     *  @copydoc AbstractCache::checkCache
     *
     *  @par Derived Implementation Details:
     *
     *  This method implements the FIFO replacement algorithm.
     */
    virtual bool checkCache(cMessage *msg);
};

/**
 * Least Recently Used Cache Manager
 */
class LRUCache : public AbstractCache
{
  public:
    /**
     *  This is the constructor for this simulation module.
     *
     *  @param namestr (in) is the name of the module
     *  @param parent (in) is the parent of this module
     *  @param stack (in) is the size in bytes of the stack for this module
     */
    LRUCache(const char *namestr=NULL, cModule *parent=NULL, size_t stack=0);

    /**
     *  @copydoc AbstractCache::setup
     *
     *  @par Derived Implementation Details:
     *
     *  This method implements the XXX
     */
    virtual void setup(void);

    /**
     *  @copydoc AbstractCache::checkCache
     *
     *  @par Derived Implementation Details:
     *
     *  This method implements the XXX
     */
    virtual bool checkCache(cMessage *msg);
};

// PriorityLRUCache : Priority LRU Cache Manager
class PriorityLRUCache : public AbstractCache
{
  public:
    /**
     *  This is the constructor for this simulation module.
     *
     *  @param namestr (in) is the name of the module
     *  @param parent (in) is the parent of this module
     *  @param stack (in) is the size in bytes of the stack for this module
     */
    PriorityLRUCache(const char *namestr=NULL, cModule *parent=NULL, size_t stack=0);

    /**
     *  @copydoc AbstractCache::setup
     *
     *  @par Derived Implementation Details:
     *
     *  This method implements the XXX
     */
    virtual void setup(void);

    /**
     *  @copydoc AbstractCache::checkCache
     *
     *  @par Derived Implementation Details:
     *
     *  This method implements the XXX
     */
    virtual bool checkCache(cMessage *msg);
};

/**
 * Fair Share Cache Manager
 */
class FairShareCache : public AbstractCache
{
  PriorityDistribution< double, 255 > the_distribution;
  public:
    /**
     *  This is the constructor for this simulation module.
     *
     *  @param namestr (in) is the name of the module
     *  @param parent (in) is the parent of this module
     *  @param stack (in) is the size in bytes of the stack for this module
     */
    FairShareCache(const char *namestr=NULL, cModule *parent=NULL, size_t stack=0);

    /**
     *  @copydoc AbstractCache::setup
     *
     *  @par Derived Implementation Details:
     *
     *  This method implements the XXX
     */
    virtual void setup(void);

    /**
     *  @copydoc AbstractCache::initializeCache
     *
     *  @par Derived Implementation Details:
     *
     *  This method implements the XXX
     */
    virtual void initializeCache(void);

    /**
     *  @copydoc AbstractCache::checkCache
     *
     *  @par Derived Implementation Details:
     *
     *  This method implements the XXX
     */
    virtual bool checkCache(cMessage *msg);
};

/**
 * Priority LRU Cache Manager with Priority Inheritance
 */
class PIPriorityLRUCache : public AbstractCache
{
  public:
    /**
     *  This is the constructor for this simulation module.
     *
     *  @param namestr (in) is the name of the module
     *  @param parent (in) is the parent of this module
     *  @param stack (in) is the size in bytes of the stack for this module
     */
    PIPriorityLRUCache(const char *namestr=NULL, cModule *parent=NULL, size_t stack=0);

    /**
     *  @copydoc AbstractCache::setup
     *
     *  @par Derived Implementation Details:
     *
     *  This method implements the XXX
     */
    virtual void setup(void);

    /**
     *  @copydoc AbstractCache::checkCache
     *
     *  @par Derived Implementation Details:
     *
     *  This method implements the XXX
     */
    virtual bool checkCache(cMessage *msg);
};

/**
 * Fair Share with Priority Inheritance Cache Manager
 */
class PIFairShareCache : public AbstractCache
{
  PriorityDistribution< double, 255 > the_distribution;
  public:
    /**
     *  This is the constructor for this simulation module.
     *
     *  @param namestr (in) is the name of the module
     *  @param parent (in) is the parent of this module
     *  @param stack (in) is the size in bytes of the stack for this module
     */
    PIFairShareCache(const char *namestr=NULL, cModule *parent=NULL, size_t stack=0);

    /**
     *  @copydoc AbstractCache::setup
     *
     *  @par Derived Implementation Details:
     *
     *  This method implements the XXX
     */
    virtual void setup(void);

    /**
     *  @copydoc AbstractCache::initializeCache
     *
     *  @par Derived Implementation Details:
     *
     *  This method implements the XXX
     */
    virtual void initializeCache(void);

    /**
     *  @copydoc AbstractCache::checkCache
     *
     *  @par Derived Implementation Details:
     *
     *  This method implements the XXX
     */
    virtual bool checkCache(cMessage *msg);
};

#endif
