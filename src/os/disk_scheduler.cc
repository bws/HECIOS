/**
 * @file disk_scheduler.cc
 *
 * Note: These classes are adapted from the FSS simulator project and are
 * licensed only under the GPL.  The FSS project is available at:
 * http://www.omnetpp.org/filemgmt/singlefile.php?lid=104 and
 * http://www.omnetpp.org/doc/FSS-doc/neddoc/index.html
 */

#include <limits.h>
#include <omnetpp.h>
#include "block_compare.h"
#include "disk_scheduler.h"

AbstractScheduler::
    AbstractScheduler( const char *namestr, cModule *parent, size_t stack):
cSimpleModule( namestr, parent, stack),
queue("disk-scheduler-queue"),
nstep_queue("disk-scheduler-nstep-queue")
{
  priorityInversionStats = new cStdDev( "scheduler-pri-inversion-stats" );
  priorityInversionLog = new cOutVector( "scheduler-pri-inversion-log", 2 );
  processTimeLog = new cOutVector( "scheduler-service-time-log", 2 );

  cPeriodMessage = new cMessage( "period-marker" );
}

AbstractScheduler::~AbstractScheduler()
{
  delete priorityInversionStats;
  delete priorityInversionLog;
  delete processTimeLog;
}

void AbstractScheduler::initialize()
{
  newReqestId = gate("in")->id();
  getRequestId = gate("get_next_request")->id();
  responseId = gate("response")->id();
  next_period = -99999999;
  RequestMsg = NULL;
  priorityInversionInProcess = false;
  priorityInversionStartTime = 0.0;

  period = par("period");
  number_of_ops = par("number_of_ops");

  setup();
  next_period = simTime() + period;
  if ( period )
    scheduleAt( next_period, cPeriodMessage );
}

void AbstractScheduler::finish()
{
  // Note that this may have ended a priority inversion
  checkForPriorityInversion( NULL );
  priorityInversionStats->recordScalar("Priority Inversion");
}

void AbstractScheduler::handleMessage( cMessage *msg )
{
  if ( period ) {
    if ( !strcmp(msg->name(), "period-marker") ) {
      next_period = simTime() + period;
      scheduleAt( next_period, cPeriodMessage );
      resetPeriod();
    }
  } 

  //  Now service the request
  if ( msg->arrivalGateId() == newReqestId ) {
    /**
     *  If this request is a write to the same disk block we already
     *  have a request outstanding for, then we should supercede the older
     *  of the two requests.  Whether the priority of the older or newer
     *  request is is used for the new one, is up to the individual
     *  algorithm to override.
     *
     *  Addressing this scenario seems like a matter of correctness!!
     *  The older data has to replaced by the newer data regardless.
     */

    checkIfWriteSatisfies( msg );

    //  Place new request on the queue
    msg->addPar("scheduler-entry-time").setDoubleValue(simTime());
    insertNewRequest( msg );

  } else if ( msg->arrivalGateId() == getRequestId ) {
    RequestMsg = msg;
  } else if ( msg->arrivalGateId() == responseId ) {

    //  If this was a read request that was just completed, there
    //  might be another read that is outstanding that could be
    //  satisfied.  This is where the check has to be performed.
    //  This is a minor algorithmic optimization/variation that
    //  not all scheduler algorithms necessarily support.

    checkIfReadSatisfies( msg );
    send( msg, "out" );
    return;
  }

  if ( RequestMsg && (msg = getNextToDo()) ) {
    // By logging here, we can track how long the request was on the queue
    processTimeLog->record(
     (double) msg->par("priority").longValue(),
     simTime() - msg->par("scheduler-entry-time").doubleValue()
    );
    //  ev << msg->par("priority").longValue() << " " <<
    //   simTime() << " " <<
    //   msg->par("scheduler-entry-time").doubleValue() << endl;

    // Check for Priority Inversion
    checkForPriorityInversion( msg );

    // Now send the request to the disk.
    ev << className() << ": To Disk " << msg->name() << endl;
    send( msg, "next_request" );
    RequestMsg->setName( "send-another-one" );
    send( RequestMsg, "next_request" );
    RequestMsg = NULL;
  }
}

//  AbstractScheduler::checkIfWriteSatisfies
//
//  By default, we want an incoming write to supercede an outstanding write
//  or read request.

void AbstractScheduler::checkIfWriteSatisfies(cMessage *msg)
{
  checkIfWriteSatisfiesAnyRequest( msg );
}

// AbstractScheduler::checkIfReadSatisfies
//
//  By default, we want a satisfied read to also satisfy any outstanding
//  read of the same block.

void AbstractScheduler::checkIfReadSatisfies(cMessage *msg)
{
  checkForSatisfiedRead( msg );
}

// AbstractScheduler::checkIfWriteSatisfiesAnyRequest
//
//  This routine is given the information on a write that was just requested.
//  If a write request is outstanding to the same disk block, then we have
//  to supercede the older request.  Technically, the older request is now
//  complete.  Similarly, a write of new data can satisfy an outstanding
//  read request of a block.

void AbstractScheduler::checkIfWriteSatisfiesAnyRequest( cMessage *msg )
{
  // Check the primary queue
  checkAQueue( &queue, msg, true, true );

  // Check the n-step queue
  checkAQueue( &nstep_queue, msg, true, true );
}

// AbstractScheduler::checkIfWriteSatisfiesAnotherWrite
//
//  This routine is given the information on a write that was just requested.
//  If a write request is outstanding to the same disk block, then we have
//  to supercede the older request.  Technically, that request is now complete.
//
//  NOTE: This routine does not consider a read satisifed by a subsequent
//        write to the same block.  checkIfWriteSatisfiesAnyRequest()
//        considers a pending read request satisfied by a subsequent write.

void AbstractScheduler::checkIfWriteSatisfiesAnotherWrite( cMessage *msg )
{
  // Check the primary queue
  checkAQueue( &queue, msg, false, true );

  // Check the n-step queue
  checkAQueue( &nstep_queue, msg, false, true );
}

// AbstractScheduler::checkForSatisfiedRead
//
//  This routine is given the information on a read that was just completed.
//  If that completed read can satisfy another outstanding read request,
//  then we will also send it back.

void AbstractScheduler::checkForSatisfiedRead( cMessage *msg )
{
  // Check the primary queue
  checkAQueue( &queue, msg, true, false );

  // Check the n-step queue
  checkAQueue( &nstep_queue, msg, true, false );
}

// AbstractScheduler::checkAQueue
//
//  This routine checks a queue for read or writes and tries to complete
//  the operation.

void AbstractScheduler::checkAQueue(
  cQueue   *q,
  cMessage *msg,
  bool      read_satisfies,
  bool      write_satisfies
)
{
  cMessage *m;
  long block_to_find;
  long b;
  bool is_read;

  block_to_find = msg->par("block").longValue();
restart:
  for ( cQueueIterator iter(*q, 1); !iter.end() ; iter++ ) {
    m = (cMessage *) iter();
    b = m->par("block").longValue();
    if ( b != block_to_find )
      continue;

    is_read = m->par("is_read").boolValue();
    //  ev << "Checking " << b << ", " << is_read << endl;
    if ( (is_read && read_satisfies) || (!is_read && write_satisfies) ) {
      ev << className() << ": " << m->name() << " is satisfied by " << msg->name();
      if (is_read) ev << " (read)"  << endl;
      else         ev << " (write)" << endl;
      m = (cMessage *) q->remove( m );
      send( m, "out" );
      goto restart;
    }
  }
}


// AbstractScheduler::insertNewRequest
//
//  This routine allows the derived classes to control how incoming
//  request messages will be queued.  The obvious choices are that
//  messages can be placed on the primary or secondary queue.

void AbstractScheduler::insertNewRequest( cMessage *msg )
{
  ev << className() << ": Inserting " << msg->name() << endl;
  queue.insert( msg );
}

// AbstractScheduler::getNextToDo
//
//  This routine allows the derived classes to control where to
//  get the next request.  Normally this method will be overridden
//  if the behavior is to move items form the secondary to the
//  primary queue when the primary queue is empty.

cMessage *AbstractScheduler::getNextToDo( void )
{
   if ( queue.empty() )
     return NULL;
   return (cMessage *)queue.pop();
}

// AbstractScheduler::checkForPriorityInversion
//
//  This routine is passed the next request that is going to be serviced.
//  It then evaluates whether or not that is one of the most important (i.e.
//  highest priority) requests outstanding.

void AbstractScheduler::checkForPriorityInversion( cMessage *msg )
{
  cMessage *m;
  //bws bool inversionFound = false;
  long priority;
  long p;
  double l;

  // Force an end to an inversion underway

  if ( !msg ) {
    if ( !priorityInversionInProcess )
      return;
    goto end_of_inversion;
  }

  priority =  msg->par("priority").longValue();
  // check the main queue
  {
    for ( cQueueIterator iter(queue, 1); !iter.end() ; iter++ ) {
      m = (cMessage *) iter();
      p = m->par("priority").longValue();
      if ( p < priority )
        goto found;
    }
  }

  // check the nstep queue
  {
    for ( cQueueIterator iter(nstep_queue, 1); !iter.end() ; iter++ ) {
      m = (cMessage *) iter();
      p = m->par("priority").longValue();
      if ( p < priority )
        goto found;
    }
  }

  // There is currently no priority inversion.  If there was not a priority
  // inversion currently happening, then we can quit.

  if ( !priorityInversionInProcess )
    return;

  // If we get here, then a priority inversion has just ended.

end_of_inversion:
  l = simTime() - priorityInversionStartTime;
  priorityInversionStats->collect( l );
  priorityInversionLog->record( priorityInversionStartTime, l );
  priorityInversionInProcess = false;
  ev << className() << ": Priority inversion ending at " << simTime() << endl;
  return;

found:
  // There is a priority inversion.  If it is one still happening, then
  // we have nothing else to record.

  if ( priorityInversionInProcess )
    return;

  //  This case should be a new case of priority inversion.

  ev << className() << ": Priority inversion starting at " << simTime() << endl;
  priorityInversionInProcess = true;
  priorityInversionStartTime = simTime();
}

//------------------------------------------------
//
//  FIFOScheduler
//
//  Simple FIFO Disk Scheduler

Define_Module_Like( FIFOScheduler, ADiskScheduler )

FIFOScheduler::
    FIFOScheduler( const char *namestr, cModule *parent, size_t stack):
AbstractScheduler( namestr, parent, stack)
{
}

//------------------------------------------------
//
//  SSTFScheduler
//
//  Shortest Seek Time First Disk Scheduler

Define_Module_Like( SSTFScheduler, ADiskScheduler )

SSTFScheduler::
    SSTFScheduler( const char *namestr, cModule *parent, size_t stack):
AbstractScheduler( namestr, parent, stack)
{
  current_block = 0;
}

cMessage *SSTFScheduler::getNextToDo( void )
{
  cMessage *msg;
  cMessage *best_msg = NULL;
  long shortest_distance = LONG_MAX;
  long d;

  if ( queue.empty() )
    return NULL;

  // Now grab as many messages as we can at the highest priority
  // that has allocated units left to do.

  for ( cQueueIterator iter(queue, 1); !iter.end() ; iter++ ) {
    msg = (cMessage *) iter();
    d = abs(current_block - msg->par("block").longValue());

    if ( d < shortest_distance ) {
       best_msg = msg;
       shortest_distance = d;
    }

  }

  //  This should never happen, but ...

  if ( !best_msg )
    return NULL;

  best_msg = (cMessage *) queue.remove( best_msg );
  return best_msg;
}

//------------------------------------------------
//
//  CScanScheduler
//
//  CSCAN Disk Scheduler -- Each sweep of the disk is in the same
//  direction.  In this implementation, the block with the lowest
//  number is serviced first.

Define_Module_Like( CScanScheduler, ADiskScheduler )

CScanScheduler::
    CScanScheduler( const char *namestr, cModule *parent, size_t stack):
AbstractScheduler( namestr, parent, stack)
{
}

void CScanScheduler::setup()
{
  queue.setup( CompareBlocksNumber, true );
}

//------------------------------------------------
//
//  NStepCScanScheduler
//
//  NStep CSCAN Disk Scheduler -- Each sweep of the disk is in the same
//  direction.  In this implementation, the block with the lowest
//  number is serviced first.  By adding NStep, the possibility of
//  indefinite postponement is reduced.
//
//  Implementation Notes:
//
//  This algorithm uses both queues.

Define_Module_Like( NStepCScanScheduler, ADiskScheduler )

NStepCScanScheduler::
    NStepCScanScheduler( const char *namestr, cModule *parent, size_t stack):
AbstractScheduler( namestr, parent, stack)
{
}

void NStepCScanScheduler::setup()
{
  queue.setup( CompareBlocksNumber, false );
  nstep_queue.setup( NULL, true );
}

void NStepCScanScheduler::insertNewRequest( cMessage *msg )
{
  //  If servicing the primary queue, then put the new request
  //  on the secondary queue.

  ev << className() << ": Inserting " << msg->name() << " on ";
  nstep_queue.insert( msg );
  ev << "secondary queue" << endl;
}

cMessage *NStepCScanScheduler::getNextToDo( void )
{
  cMessage *msg;

  if ( queue.empty() ) {
    if ( nstep_queue.empty() )
      return NULL;

    // ev << className() << ": Switching to primary queue" << endl;

    while ( !nstep_queue.empty() ) {
      msg = (cMessage *) nstep_queue.pop();
      ev << className() << ": Moving " << msg->name() << endl;
      queue.insert( msg );
    }
  }

  return (cMessage *)queue.pop();
}


//------------------------------------------------
//
//  ScanScheduler
//
//  SCAN Disk Scheduler -- each sweep of the disk is in a different
//  direction.  On one pass, the block with the lowest number is
//  serviced first.  On the next pass, the block with the highest
//  number is serviced.

Define_Module_Like( ScanScheduler, ADiskScheduler )

ScanScheduler::
    ScanScheduler( const char *namestr, cModule *parent, size_t stack):
AbstractScheduler( namestr, parent, stack)
{
}

void ScanScheduler::setup()
{
  going_up = false;
}

void ScanScheduler::insertNewRequest( cMessage *msg )
{
  if ( queue.empty() ) {

    ev << className() << ": Switching directions from " ;
    if ( going_up ) ev << "UP";
    else            ev << "DOWN";

    going_up = (going_up) ? false : true;
    queue.setup( CompareBlocksNumber, !going_up );

    ev << " to ";
    if ( going_up ) ev << "UP";
    else            ev << "DOWN";
    ev << endl;
  }

  ev << className() << ": Inserting " << msg->name() << endl;
  queue.insert( msg );
}

// ScanScheduler::getNextToDo
//
//  Behavior is the same as AbstractScheduler


//------------------------------------------------
//
//  NStepScanScheduler
//
//  N-Step SCAN Disk Scheduler -- each sweep of the disk is in a different
//  direction.  On one pass, the block with the lowest number is
//  serviced first.  On the next pass, the block with the highest
//  number is serviced.
//
//  Implementation Notes:
//
//  This algorithm uses both queues.

Define_Module_Like( NStepScanScheduler, ADiskScheduler )

NStepScanScheduler::
    NStepScanScheduler( const char *namestr, cModule *parent, size_t stack):
AbstractScheduler( namestr, parent, stack)
{
}

void NStepScanScheduler::setup()
{
  going_up = false;
  nstep_queue.setup( NULL, true );
}

void NStepScanScheduler::insertNewRequest( cMessage *msg )
{
  //  If servicing the primary queue, then put the new request
  //  on the secondary queue.

  ev << className() << ": Inserting " << msg->name() << " on ";
  nstep_queue.insert( msg );
  ev << "secondary queue" << endl;
}

cMessage *NStepScanScheduler::getNextToDo( void )
{
  cMessage *msg;

  if ( queue.empty() ) {
    if ( nstep_queue.empty() )
      return NULL;

    // ev << className() << ": Switching to primary queue" << endl;

    ev << className() << ": Switching directions from " ;
    if ( going_up ) ev << "UP";
    else            ev << "DOWN";

    going_up = (going_up) ? false : true;
    queue.setup( CompareBlocksNumber, !going_up );

    ev << " to ";
    if ( going_up ) ev << "UP";
    else            ev << "DOWN";
    ev << endl;

    while ( !nstep_queue.empty() ) {
      msg = (cMessage *) nstep_queue.pop();
      ev << className() << ": Moving " << msg->name() << endl;
      queue.insert( msg );
    }
  }

  return (cMessage *)queue.pop();
}

//------------------------------------------------
//
//  PriorityScheduler
//
//  Priority Disk Scheduler -- Each sweep of the disk is in the same
//  direction.  In this implementation, the block with the lowest
//  number is serviced first.

Define_Module_Like( PriorityScheduler, ADiskScheduler )

PriorityScheduler::
    PriorityScheduler( const char *namestr, cModule *parent, size_t stack):
AbstractScheduler( namestr, parent, stack)
{
}

void PriorityScheduler::setup()
{
  queue.setup( CompareBlocksPriority, false );
}

//------------------------------------------------
//
//  PriorityScanScheduler
//
//  Priority SCAN Disk Scheduler -- Each sweep of the disk is
//  in a different direction with higher priority requests serviced first.
//  On one pass, the block with the lowest number is serviced first.
//  On the next pass, the block with the highest number is serviced.

Define_Module_Like( PriorityScanScheduler, ADiskScheduler )

PriorityScanScheduler::
    PriorityScanScheduler( const char *namestr, cModule *parent, size_t stack):
AbstractScheduler( namestr, parent, stack)
{
}

void PriorityScanScheduler::setup()
{
  going_up = false;
}

void PriorityScanScheduler::insertNewRequest( cMessage *msg )
{
  if ( queue.empty() ) {

    ev << className() << ": Switching directions from " ;
    if ( going_up ) ev << "UP";
    else            ev << "DOWN";

    going_up = (going_up) ? false : true;
    if ( going_up )
      queue.setup( CompareBlocksPriorityAscending, false );
    else
      queue.setup( CompareBlocksPriorityDescending, false );

    ev << " to ";
    if ( going_up ) ev << "UP";
    else            ev << "DOWN";
    ev << endl;
  }

  ev << className() << ": Inserting " << msg->name() << endl;
  queue.insert( msg );
}

// PriorityScanScheduler::getNextToDo
//
//  Behavior is the same as AbstractScheduler

//------------------------------------------------
//
//  PriorityCScanScheduler
//
//  Priority CSCAN Disk Scheduler -- Each sweep of the disk is in the same
//  direction.  In this implementation, the block with the lowest
//  number is serviced first.

Define_Module_Like( PriorityCScanScheduler, ADiskScheduler )

PriorityCScanScheduler::
    PriorityCScanScheduler( const char *namestr, cModule *parent, size_t stack):
AbstractScheduler( namestr, parent, stack)
{
}

void PriorityCScanScheduler::setup()
{
  queue.setup( CompareBlocksPriorityAscending, false );
}

//------------------------------------------------
//
//  PriorityNStepScanScheduler
//
//  Priority N-Step SCAN Disk Scheduler
//
//  Each sweep of the disk is in a different direction.  On each sweep
//  of the disk only the most important set of requests outstanding at
//  the beginning of the sweep is serviced.
//
//  On one pass, the block with the lowest number is serviced first.
//  On the next pass, the block with the highest number is serviced.
//
//  Implementation Notes:
//
//  This algorithm uses both queues.

Define_Module_Like( PriorityNStepScanScheduler, ADiskScheduler )

PriorityNStepScanScheduler::
    PriorityNStepScanScheduler( const char *namestr, cModule *parent, size_t stack):
AbstractScheduler( namestr, parent, stack)
{
}

void PriorityNStepScanScheduler::setup()
{
  going_up = false;
  nstep_queue.setup( CompareBlocksPriority, false );
}

void PriorityNStepScanScheduler::insertNewRequest( cMessage *msg )
{
  //  Put the new request on the secondary queue.
  //  It will get picked up next time we are asked for something to do.

  ev << className() << ": Inserting " << msg->name() << " on ";
  nstep_queue.insert( msg );
  ev << "secondary queue" << endl;
}

cMessage *PriorityNStepScanScheduler::getNextToDo( void )
{
  cMessage *msg;
  int priority;

  if ( queue.empty() ) {
    if ( nstep_queue.empty() )
      return NULL;

    // ev << className() << ": Switching to primary queue" << endl;

    ev << className() << ": Switching directions from " ;
    if ( going_up ) ev << "UP";
    else            ev << "DOWN";

    going_up = (going_up) ? false : true;
    if ( going_up )
      queue.setup( CompareBlocksPriorityAscending, false );
    else
      queue.setup( CompareBlocksPriorityDescending, false );

    ev << " to ";
    if ( going_up ) ev << "UP";
    else            ev << "DOWN";
    ev << endl;

    // first message is special since it sets the priority for this pass
    msg = (cMessage *) nstep_queue.pop();
    priority = msg->par("priority").longValue();
    nstep_queue.insert( msg );

    while ( !nstep_queue.empty() ) {
      msg = (cMessage *) nstep_queue.pop();
      if ( priority != msg->par("priority").longValue() ) {
        nstep_queue.insert( msg );
        break;
      }
      ev << className() << ": Moving " << msg->name() << endl;
      queue.insert( msg );
    }
  }

  return (cMessage *)queue.pop();
}

//------------------------------------------------
//
//  FairShareScheduler
//
//  Fair Share Disk Scheduler
//
//  The number of operations at each task priority is limited by
//  the user.  At a user defined interval, the accounting information
//  is reset.  For example, the system configuration manager specifies
//  the following information:
//
//      + Length of each time unit
//      + Total disk operations per unit
//      + Percentage of operations per priority
//
//  Usually the number of total disk operations possible is worst case.
//  Since disks have a non-linear access time, the worst case movement
//  should be the basis for the schedule in a real-time system.  This
//  will normally result in extra time to perform non-real-time disk
//  operations when there are no real-time request pending.
//
//  Each sweep of the disk is in a different direction.  On each sweep
//  of the disk only the most important set of requests outstanding at
//  the beginning of the sweep is serviced.  Preference is given to
//  the highest priority requests for priorities that have not exceeded
//  their allocation.
//
//  On one pass, the block with the lowest number is serviced first.
//  On the next pass, the block with the highest number is serviced.
//
//  The simplest way to look at this algorithm is that it is
//  Priority N-Step SCAN with the addition of the limits at each priority.
//
//  Implementation Notes:
//
//  This algorithm uses both queues.

Define_Module_Like( FairShareScheduler, ADiskScheduler )

FairShareScheduler::
    FairShareScheduler( const char *namestr, cModule *parent, size_t stack):
AbstractScheduler( namestr, parent, stack)
{
}

void FairShareScheduler::setup()
{
  long reserved_ops_at_priority;
  double share;
  const char *filename = par("scheduler_file").stringValue();

  going_up = false;

  nstep_queue.setup( CompareBlocksPriority, true );

  the_distribution.loadFromFile(filename);
  the_count.initialize();
  the_limit.initialize();

  for ( int p=1 ; p<=the_distribution.getMaximumPriority() ; p++ ) {

    share = the_distribution.getElement( p );
    reserved_ops_at_priority = (int) ((double)number_of_ops * share);

    the_limit.setElement( p, reserved_ops_at_priority );
  }
}

void FairShareScheduler::resetPeriod( void )
{
  ev << className() << ": Reset " << simTime() << endl;
  the_count.initialize();
}

void FairShareScheduler::insertNewRequest( cMessage *msg )
{
  //  Put the new request on the secondary queue.
  //  It will get picked up next time we are asked for something to do.

  ev << className() << ": Inserting " << msg->name() << " on ";
  nstep_queue.insert( msg );
  ev << "secondary queue" << endl;
}

cMessage *FairShareScheduler::getNextToDo( void )
{
  cMessage *msg;
  int p;

  if ( queue.empty() ) {
    if ( nstep_queue.empty() )
      return NULL;

    // ev << className() << ": Switching to primary queue" << endl;

    ev << className() << ": Switching directions from " ;
    if ( going_up ) ev << "UP";
    else            ev << "DOWN";

    going_up = (going_up) ? false : true;
    if ( going_up )
      queue.setup( CompareBlocksPriorityAscending, true );
    else
      queue.setup( CompareBlocksPriorityDescending, true );

    ev << " to ";
    if ( going_up ) ev << "UP";
    else            ev << "DOWN";
    ev << endl;

    // Now grab as many messages as we can at the highest priority
    // that has allocated units left to do.

    for ( cQueueIterator iter(nstep_queue, 1); !iter.end() ; iter++ ) {
      msg = (cMessage *) iter();
      p = msg->par("priority").longValue();

      //  ev << p << " (CHECK) has used " << the_count.getElement(p) <<
      //    " of " << the_limit.getElement(p)  << endl;
      if ( the_count.getElement(p) < the_limit.getElement(p) ) {
        for ( ;; ) {
          iter++;
          msg = (cMessage *) nstep_queue.remove( msg );
          ev << className() << ": Moving " << msg->name() << endl;
          queue.insert( msg );
          the_count.setElement(p, the_count.getElement(p) + 1 );

           // ev << p << " (ADD) has used " << the_count.getElement(p) <<
           //   " of " << the_limit.getElement(p)  << endl;
          if ( the_count.getElement(p) >= the_limit.getElement(p) )
            goto done;
          if ( iter.end() )
            goto done;
          msg = (cMessage *) iter();
          if ( p != msg->par("priority").longValue() )
            goto done;
        }
      }
    }

    // Sigh .. all the priorities that have outstanding requests
    // have used their allocation for this period.  So just grab
    // the highest priority request to do.  This is not that efficient
    // but it minimizes the damage of moving multiple operations
    // to the queue in case a new operation that has units left shows up.

    msg = (cMessage *) nstep_queue.remove(nstep_queue.head());
    ev << className() << ": Moving " << msg->name() << endl;
    p = msg->par("priority").longValue();
    the_count.setElement(p, the_count.getElement(p) + 1 );
    queue.insert( msg );
  }

done:
  msg = (cMessage *)queue.pop();
  p = msg->par("priority").longValue();
  ev << className() << ": " << the_count.getElement(p) <<
        " accesses at priority " << p << endl;
  return msg;
}

//------------------------------------------------
//
//  PreemptivePriorityNStepScanScheduler
//
//  Priority N-Step SCAN Disk Scheduler
//
//  This algorithm is the same as Priority N-Step SCAN Disk Scheduler
//  except that if an incoming request is of higher priority than
//  the ones being serviced, there will be a preemption.
//
//  Implementation Notes:
//
//  This algorithm uses both queues.

Define_Module_Like( PreemptivePriorityNStepScanScheduler, ADiskScheduler )

PreemptivePriorityNStepScanScheduler::
    PreemptivePriorityNStepScanScheduler(
       const char *namestr, cModule *parent, size_t stack):
PriorityNStepScanScheduler( namestr, parent, stack)
{
}

void PreemptivePriorityNStepScanScheduler::insertNewRequest( cMessage *msg )
{

  //  If there are outstanding requests on the primary queue and the
  //  incoming request is of higher priority, then preempt the current
  //  sweep of the disk.  As soon as the request currently being processed
  //  is completed, we will begin processing those of the new priority.

  if ( !queue.empty() ) {
    cQueueIterator iter(queue, 1);
    cMessage *m = (cMessage *) iter();
    if ( CompareBlocksPriority( msg, m ) < 0 ) {
      ev << className() << ": preempting because of " <<
           msg->name() << " incoming and " << m->name() << " on queue " << endl;
      while( !queue.empty() ) {
        m = (cMessage *)queue.pop();
        nstep_queue.insert( m );
      }
    }
  }

  //  Put the new request on the secondary queue.
  //  It will get picked up next time we are asked for something to do.

  ev << className() << ": Inserting " <<
       msg->name() << " on ";
  nstep_queue.insert( msg );
  ev << "secondary queue" << endl;
}

//------------------------------------------------
//
//  PreemptiveFairShareScheduler
//
//  Priority Fair Share Scheduler
//
//  This algorithm is the same as Fair Share Disk Scheduler
//  except that if an incoming request is of higher priority than
//  the ones being serviced, there will be a preemption.
//
//  Implementation Notes:
//
//  This algorithm uses both queues.

Define_Module_Like( PreemptiveFairShareScheduler, ADiskScheduler )

PreemptiveFairShareScheduler::PreemptiveFairShareScheduler(
       const char *namestr, cModule *parent, size_t stack):
AbstractScheduler( namestr, parent, stack)
{
}

void PreemptiveFairShareScheduler::setup()
{
  long reserved_ops_at_priority;
  double share;
  const char *filename = par("scheduler_file").stringValue();

  going_up = false;

  nstep_queue.setup( CompareBlocksPriority, true );

  the_distribution.loadFromFile(filename);
  the_count.initialize();
  the_limit.initialize();

  for ( int p=1 ; p<=the_distribution.getMaximumPriority() ; p++ ) {

    share = the_distribution.getElement( p );
    reserved_ops_at_priority = (int) ((double)number_of_ops * share);

    the_limit.setElement( p, reserved_ops_at_priority );
  }
}

void PreemptiveFairShareScheduler::resetPeriod( void )
{
  ev << className() << ": Reset " << simTime() << endl;
  the_count.initialize();
}

void PreemptiveFairShareScheduler::insertNewRequest( cMessage *msg )
{
  int p;

  //  If there are outstanding requests on the primary queue and the
  //  incoming request is of higher priority, then preempt the current
  //  sweep of the disk.  As soon as the request currently being processed
  //  is completed, we will begin processing those of the new priority.

  if ( !queue.empty() ) {
    cQueueIterator iter(queue, 1);
    cMessage *m = (cMessage *) iter();
    if ( CompareBlocksPriority( msg, m ) < 0 ) {
      ev << className() << ": preempting because of " <<
           msg->name() << " incoming and " << m->name() << " on queue " << endl;
      while( !queue.empty() ) {
        m = (cMessage *)queue.pop();
        p = m->par("priority").longValue();
        if ( the_count.getElement(p) )
          the_count.setElement(p, the_count.getElement(p) - 1 );
        //  ev << p << " (SUB) has used " << the_count.getElement(p) <<
        //    " of " << the_limit.getElement(p)  << endl;
        nstep_queue.insert( m );
      }
    }
  }

  //  Put the new request on the secondary queue.
  //  It will get picked up next time we are asked for something to do.

  ev << className() << ": Inserting " <<
       msg->name() << " on ";
  nstep_queue.insert( msg );
  ev << "secondary queue" << endl;
}

cMessage *PreemptiveFairShareScheduler::getNextToDo( void )
{
  cMessage *msg;
  int p;

  if ( queue.empty() ) {
    if ( nstep_queue.empty() )
      return NULL;

    // ev << className() << ": Switching to primary queue" << endl;

    ev << className() << ": Switching directions from " ;
    if ( going_up ) ev << "UP";
    else            ev << "DOWN";

    going_up = (going_up) ? false : true;
    if ( going_up )
      queue.setup( CompareBlocksPriorityAscending, true );
    else
      queue.setup( CompareBlocksPriorityDescending, true );

    ev << " to ";
    if ( going_up ) ev << "UP";
    else            ev << "DOWN";
    ev << endl;

    // Now grab as many messages as we can at the highest priority
    // that has allocated units left to do.

    for ( cQueueIterator iter(nstep_queue, 1); !iter.end() ; iter++ ) {
      msg = (cMessage *) iter();
      p = msg->par("priority").longValue();

      //  ev << p << " (CHECK) has used " << the_count.getElement(p) <<
      //     " of " << the_limit.getElement(p)  << endl;
      if ( the_count.getElement(p) < the_limit.getElement(p) ) {
        for ( ;; ) {
          iter++;
          msg = (cMessage *) nstep_queue.remove( msg );
          ev << className() << ": Moving " << msg->name() << endl;
          queue.insert( msg );
          the_count.setElement(p, the_count.getElement(p) + 1 );

          //  ev << p << " (ADD) has used " << the_count.getElement(p) <<
          //    " of " << the_limit.getElement(p)  << endl;
          if ( the_count.getElement(p) >= the_limit.getElement(p) )
            goto done;
          if ( iter.end() )
            goto done;
          msg = (cMessage *) iter();
          if ( p != msg->par("priority").longValue() )
            goto done;
        }
      }
    }

    // Sigh .. all the priorities that have outstanding requests
    // have used their allocation for this period.  So just grab
    // the highest priority request to do.  This is not that efficient
    // but it minimizes the damage of moving multiple operations
    // to the queue in case a new operation that has units left shows up.

    msg = (cMessage *) nstep_queue.remove(nstep_queue.head());
    ev << className() << ": Moving " << msg->name() << endl;
    p = msg->par("priority").longValue();
    the_count.setElement(p, the_count.getElement(p) + 1 );
    queue.insert( msg );
  }

done:
  msg = (cMessage *)queue.pop();
  p = msg->par("priority").longValue();
  ev << className() << ": " << the_count.getElement(p) <<
        " accesses at priority " << p << endl;
  return msg;
}

