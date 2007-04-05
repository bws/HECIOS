#ifndef DISK_SCHEDULER_H
#define DISK_SCHEDULER_H
/**
 * @file disk_scheduler.h
 * @brief Disk Scheduler Modules
 *
 * Note: These classes are adapted from the FSS simulator project and are
 * licensed only under the GPL.  The FSS project is available at:
 * http://www.omnetpp.org/filemgmt/singlefile.php?lid=104 and
 * http://www.omnetpp.org/doc/FSS-doc/neddoc/index.html
 */

#include <omnetpp.h>
#include "priority_distribution.h"

// This is the set of block comparison functions available to the
// various disk scheduling algorithms.
extern "C" {
  int CompareBlocksNumber(cObject *left, cObject *right);
  int CompareBlocksTimeStamp(cObject *left, cObject *right);
  int CompareBlocksPriority(cObject *left, cObject *right);
  int CompareBlocksPriorityAscending(cObject *left, cObject *right);
  int CompareBlocksPriorityDescending(cObject *left, cObject *right);
}; /* end of extern "C" */

/**
 * Abstract base class for Disk Schedulers
 */
class AbstractScheduler : public cSimpleModule
{
  public:
    /**
     *  This is the constructor for this simulation module.
     *
     *  @param namestr (in) is the name of the module
     *  @param parent (in) is the parent of this module
     *  @param stack (in) is the size in bytes of the stack for this module
     */
    AbstractScheduler(const char *namestr=NULL, cModule *parent=NULL, size_t stack=0);

    /**
     *  This is the destructor for this simulation module.
     */
    ~AbstractScheduler();

    int       number_of_ops;   // number of ops to schedule per period

    double    period;          // length of each period

    cQueue    queue;

    cQueue    nstep_queue;

    cMessage *cPeriodMessage;

    int newReqestId;

    int getRequestId;

    int responseId;

    double next_period;

    cMessage *RequestMsg;

    bool        priorityInversionInProcess;

    double      priorityInversionStartTime;

    cStdDev    *priorityInversionStats;

    cOutVector *priorityInversionLog;

    cOutVector *processTimeLog;

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

    void checkIfWriteSatisfiesAnyRequest(cMessage *msg);

    void checkIfWriteSatisfiesAnotherWrite(cMessage *msg);

    void checkForSatisfiedRead(cMessage *msg);

    void checkAQueue(
      cQueue *q, cMessage *msg, bool read_satisfies, bool write_satisfies);

    // hook functions to (re)define behaviour
    virtual void setup(void) {};

    virtual void resetPeriod(void) {};

    virtual void checkIfWriteSatisfies(cMessage *msg);

    virtual void checkIfReadSatisfies(cMessage *msg);

    virtual void insertNewRequest(cMessage *msg);

    virtual cMessage *getNextToDo();

    virtual void checkForPriorityInversion(cMessage *msg);
};

/**
 * FIFO Disk Scheduler
 */
class FIFOScheduler : public AbstractScheduler
{
  public:
    /**
     *  This is the constructor for this simulation module.
     *
     *  @param namestr (in) is the name of the module
     *  @param parent (in) is the parent of this module
     *  @param stack (in) is the size in bytes of the stack for this module
     */
    FIFOScheduler(const char *namestr=NULL, cModule *parent=NULL, size_t stack=0);
};

/**
 * Shortest Seek Time First Disk Scheduler
 */
class SSTFScheduler : public AbstractScheduler
{
  long current_block;

  public:
    /**
     *  This is the constructor for this simulation module.
     *
     *  @param namestr (in) is the name of the module
     *  @param parent (in) is the parent of this module
     *  @param stack (in) is the size in bytes of the stack for this module
     */
    SSTFScheduler(const char *namestr=NULL, cModule *parent=NULL, size_t stack=0);

    virtual cMessage *getNextToDo();
};

/**
 * C-SCAN Disk Scheduler
 */
class CScanScheduler : public AbstractScheduler
{
  public:
    /**
     *  This is the constructor for this simulation module.
     *
     *  @param namestr (in) is the name of the module
     *  @param parent (in) is the parent of this module
     *  @param stack (in) is the size in bytes of the stack for this module
     */
    CScanScheduler(const char *namestr=NULL, cModule *parent=NULL, size_t stack=0);

    virtual void setup( void );
};

/**
 * N-Step C-SCAN Disk Scheduler
 */
class NStepCScanScheduler : public AbstractScheduler
{
  public:
    /**
     *  This is the constructor for this simulation module.
     *
     *  @param namestr (in) is the name of the module
     *  @param parent (in) is the parent of this module
     *  @param stack (in) is the size in bytes of the stack for this module
     */
    NStepCScanScheduler(const char *namestr=NULL, cModule *parent=NULL, size_t stack=0);

    virtual void setup( void );

    virtual void insertNewRequest( cMessage *msg );

    virtual cMessage *getNextToDo();
};


/**
 * SCAN Disk Scheduler
 */
class ScanScheduler : public AbstractScheduler
{
    bool going_up;
  public:
    /**
     *  This is the constructor for this simulation module.
     *
     *  @param namestr (in) is the name of the module
     *  @param parent (in) is the parent of this module
     *  @param stack (in) is the size in bytes of the stack for this module
     */
    ScanScheduler(const char *namestr=NULL, cModule *parent=NULL, size_t stack=0);

    virtual void setup( void );

    virtual void insertNewRequest( cMessage *msg );
};

/**
 * N-Step SCAN Disk Scheduler
 */
class NStepScanScheduler : public AbstractScheduler
{
    bool going_up;
  public:
    /**
     *  This is the constructor for this simulation module.
     *
     *  @param namestr (in) is the name of the module
     *  @param parent (in) is the parent of this module
     *  @param stack (in) is the size in bytes of the stack for this module
     */
    NStepScanScheduler(const char *namestr=NULL, cModule *parent=NULL, size_t stack=0);

    virtual void setup( void );

    virtual void insertNewRequest( cMessage *msg );

    virtual cMessage *getNextToDo();
};

/**
 * Priority Disk Scheduler
 */
class PriorityScheduler : public AbstractScheduler
{
  public:
    /**
     *  This is the constructor for this simulation module.
     *
     *  @param namestr (in) is the name of the module
     *  @param parent (in) is the parent of this module
     *  @param stack (in) is the size in bytes of the stack for this module
     */
    PriorityScheduler(const char *namestr=NULL, cModule *parent=NULL, size_t stack=0);

    virtual void setup( void );
};

/**
 * Priority SCAN Disk Scheduler
 */
class PriorityScanScheduler : public AbstractScheduler
{
    bool going_up;
  public:
    /**
     *  This is the constructor for this simulation module.
     *
     *  @param namestr (in) is the name of the module
     *  @param parent (in) is the parent of this module
     *  @param stack (in) is the size in bytes of the stack for this module
     */
    PriorityScanScheduler(const char *namestr=NULL, cModule *parent=NULL, size_t stack=0);

    virtual void setup( void );

    virtual void insertNewRequest( cMessage *msg );
};

/**
 * Priority C-SCAN Disk Scheduler
 */
class PriorityCScanScheduler : public AbstractScheduler
{
    bool going_up;
  public:
    /**
     *  This is the constructor for this simulation module.
     *
     *  @param namestr (in) is the name of the module
     *  @param parent (in) is the parent of this module
     *  @param stack (in) is the size in bytes of the stack for this module
     */
    PriorityCScanScheduler(
      const char *namestr=NULL, cModule *parent=NULL, size_t stack=0);

    virtual void setup( void );
};

/**
 * Priority N-Step SCAN Disk Scheduler
 */
class PriorityNStepScanScheduler : public AbstractScheduler
{
    bool going_up;
  public:
    /**
     *  This is the constructor for this simulation module.
     *
     *  @param namestr (in) is the name of the module
     *  @param parent (in) is the parent of this module
     *  @param stack (in) is the size in bytes of the stack for this module
     */
    PriorityNStepScanScheduler(
      const char *namestr=NULL, cModule *parent=NULL, size_t stack=0);

    virtual void setup( void );

    virtual void insertNewRequest( cMessage *msg );

    virtual cMessage *getNextToDo();
};

/**
 * Fair Share Disk Scheduler
 */
class FairShareScheduler : public AbstractScheduler
{
    bool going_up;
    PriorityDistribution< double, 255 > the_distribution;
    PriorityDistribution< long, 255 > the_limit;
    PriorityDistribution< long, 255 > the_count;
    double reset_time;

  public:
    /**
     *  This is the constructor for this simulation module.
     *
     *  @param namestr (in) is the name of the module
     *  @param parent (in) is the parent of this module
     *  @param stack (in) is the size in bytes of the stack for this module
     */
    FairShareScheduler(
      const char *namestr=NULL, cModule *parent=NULL, size_t stack=0);

    virtual void setup( void );

    virtual void resetPeriod( void );

    virtual void insertNewRequest( cMessage *msg );

    virtual cMessage *getNextToDo();
};

/**
 * Priority N-Step SCAN Disk Scheduler with Preemption
 */
class PreemptivePriorityNStepScanScheduler : public PriorityNStepScanScheduler
{
    bool going_up;
  public:
    /**
     *  This is the constructor for this simulation module.
     *
     *  @param namestr (in) is the name of the module
     *  @param parent (in) is the parent of this module
     *  @param stack (in) is the size in bytes of the stack for this module
     */
    PreemptivePriorityNStepScanScheduler(
      const char *namestr=NULL, cModule *parent=NULL, size_t stack=0);

    virtual void insertNewRequest( cMessage *msg );
};

/**
 * Preemptive Fair Share Disk Scheduler
 */
class PreemptiveFairShareScheduler : public AbstractScheduler
{
    bool going_up;
    PriorityDistribution< double, 255 > the_distribution;
    PriorityDistribution< long, 255 > the_limit;
    PriorityDistribution< long, 255 > the_count;
    double reset_time;

  public:
    /**
     *  This is the constructor for this simulation module.
     *
     *  @param namestr (in) is the name of the module
     *  @param parent (in) is the parent of this module
     *  @param stack (in) is the size in bytes of the stack for this module
     */
    PreemptiveFairShareScheduler(
      const char *namestr=NULL, cModule *parent=NULL, size_t stack=0);

    virtual void setup( void );

    virtual void resetPeriod( void );

    virtual void insertNewRequest( cMessage *msg );

    virtual cMessage *getNextToDo();
};

#endif

