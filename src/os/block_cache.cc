/**
 * @file block_cache.cc
 *
 * Note: These classes are adapted from the FSS simulator project and are
 * licensed only under the GPL.  The FSS project is available at:
 * http://www.omnetpp.org/filemgmt/singlefile.php?lid=104 and
 * http://www.omnetpp.org/doc/FSS-doc/neddoc/index.html
 */

#include <limits.h>
#include <omnetpp.h>
#include "block_compare.h"
#include "block_cache.h"

//------------------------------------------------
//
//  AbstractCache
//
//  This is the base class for all Cache Management Algorithms.
//  All cache algorithm implementations manage a single set of
//  cache elements.

AbstractCache::
  AbstractCache( const char *namestr, cModule *parent, size_t stack):
cSimpleModule( namestr, parent, stack),
Cache("Cache")
{
}

void AbstractCache::finish()
{
  double hit_ratio;

  hit_ratio = (double) number_of_hits / (double) number_of_misses;
  ev << className() << ": End of Simulation" << endl;

  // ev << className() << ": Statistics" << endl;
  // ev << className() << ": Accesses: "           << number_of_requests << endl;
  // ev << className() << ": Hit Ratio:"           << hit_ratio << endl;
  // ev << className() << ": Hits:     "           << number_of_hits << endl;
  // ev << className() << ": Misses:   "           << number_of_misses << endl;

  recordScalar( "Cache requests", number_of_requests );
  recordScalar( "Cache Hit Ratio", hit_ratio );
  recordScalar( "Cache Hits", number_of_hits );
  recordScalar( "Cache Misses", number_of_misses );
  recordScalar( "Number of Priority Inversions", number_of_inversions );
}


void AbstractCache::initialize()
{
  number_of_requests   = 0;
  number_of_hits       = 0;
  number_of_misses     = 0;
  number_of_inversions = 0;

  setup();
  initializeCache();
}

void AbstractCache::handleMessage(cMessage *msg)
{
  int fromInGateId = gate("in")->id();

  //  Either the message is a request of the cache or it is a response
  //  to a disk access request made as the result of a miss.

  if ( msg->arrivalGateId() == fromInGateId ) {
    if ( msg->par("is_read").boolValue() ) {
      number_of_requests++;
      if ( !checkCache(msg) ) {
        ev << className() << ": " << msg->name() << " was not satisfied" << endl;
        send( msg, "request" );
        number_of_misses++;
      } else {
        ev << className() << ": " << msg->name() << " was satisfied " << endl;
        send( msg, "out" );
        number_of_hits++;
      }
    } else {
      checkCache(msg);
      ev << className() << ": " << msg->name() << " to disk" << endl;
      send( msg, "request" );
    }

  } else {
    //  Data is back from the disk .. mark this data as filled in
    long block_to_find = msg->par("block").longValue();

    ev << className() << ": " << msg->name() << " processed by the disk" << endl;

    for ( cQueueIterator iter(Cache, 1); !iter.end() ; iter++ ) {
      cMessage *m = (cMessage *) iter();
      long b = m->par("block").longValue();
      if ( b != block_to_find )
        continue;

      m->par("has_data").setBoolValue( true );
      m->par("is_dirty").setBoolValue( false );
    }
    send( msg, "out" );
  }
}

void AbstractCache::initializeCache()
{
  cMessage *msg;
  char msgname[32];

  number_of_elements = par("number_of_elements");

  for ( int i=0 ; i<number_of_elements ; i++ ) {
    sprintf( msgname, "cache-%d", i );
    msg = new cMessage( msgname );
    msg->addPar("block").setLongValue(LONG_MAX);
    msg->addPar("priority").setLongValue(LONG_MAX);
    msg->addPar("is_dirty").setBoolValue(false);
    msg->addPar("has_data").setBoolValue(false);
    msg->setTimestamp();
    Cache.insert( msg );
  }
}

bool AbstractCache::checkCache(cMessage *)
{
  return false;
}

// AbstractCache::checkForPriorityInversion
//
//  This routine is passed the cache element that was just evicted.
//  It then evaluates whether or not that is the one of the least
//  important (i.e. lowest priority) requests outstanding.

void AbstractCache::checkForPriorityInversion( cMessage *msg )
{
  cMessage *m;
  //bws bool inversionFound = false;
  long priority =  msg->par("priority").longValue();
  long p;

  // If this block has never been used, there is no inversion
  if ( msg->par("block").longValue() == LONG_MAX )
    return;

  // check the cache queue
  for ( cQueueIterator iter(Cache, 1); !iter.end() ; iter++ ) {
    m = (cMessage *) iter();
    p = m->par("priority").longValue();
    if ( p > priority ) {
      ev << className() << ": Priority Inversion Detected: Could have evicted (" <<
         m->par("block").longValue() << ", " << p << ")" << endl;
      number_of_inversions++;
      return;
    }
  }
}

//------------------------------------------------
//
//  NoCache
//
//  The No Cache Management Algorithm simulates having no cache at all.
//  It says that the requested element is not in the cache.

Define_Module_Like( NoCache, ACache )

NoCache::
    NoCache( const char *namestr, cModule *parent, size_t stack ):
  AbstractCache( namestr, parent, stack )
{
}

//------------------------------------------------
//
//  FIFOCache
//
//  The FIFO Cache Management Algorithm chooses the oldest element
//  in the cache for replacement.  If the same element is access
//  multiple times, the subsequent accesses will not keep the element
//  in the cache.  The LRU Algorithm takes subsequent access into account.

Define_Module_Like( FIFOCache, ACache )

FIFOCache::
    FIFOCache( const char *namestr, cModule *parent, size_t stack ):
  AbstractCache( namestr, parent, stack )
{
}

bool FIFOCache::checkCache(cMessage *msg_to_check)
{
  int             block_to_find;
  int             block;
  int             new_priority;
  cMessage       *msg;

  block_to_find = msg_to_check->par("block").longValue();
  new_priority  = msg_to_check->par("priority").longValue();
  ev << className() << ": Looking for (" << block_to_find << ", " <<
     new_priority << ")" << endl;

  for( cQueueIterator iter(Cache, 1); !iter.end() ; iter++ ) {
    msg = (cMessage *) iter();
    block = msg->par("block").longValue();

    // ev << className() << ": Checking " << block << " for " <<
    //     block_to_find << endl;
    if ( block_to_find == block ) {
      ev << className() << ": Found " << block_to_find << endl;

      if ( msg_to_check->par("is_read").boolValue() ) {
        if ( !msg->par("has_data").boolValue() )
          return false;
      } else {
        msg->par("has_data").setBoolValue( true );
        msg->par("is_dirty").setBoolValue( true );
      }
      return true;
    }
  }
  // Now replace an element
  msg = (cMessage *) Cache.pop();

  ev << className() << ": Evicting (" << msg->par("block").longValue() << ", " <<
      msg->par("priority").longValue() << ") for " << block_to_find << endl;

  // Check for Priority Inversion
  checkForPriorityInversion( msg );

  msg->setTimestamp();
  msg->par("block").setLongValue(block_to_find );
  msg->par("priority").setLongValue( new_priority );
  if ( msg_to_check->par("is_read").boolValue() ) {
    msg->par("has_data").setBoolValue( false );
    msg->par("is_dirty").setBoolValue( false );
  } else {
    msg->par("has_data").setBoolValue( true );
    msg->par("is_dirty").setBoolValue( true );
  }
  Cache.insert( msg );
  return false;
}

//------------------------------------------------
//
//  LRUCache
//
//  The LRU Cache Management Algorithm chooses the element
//  in the cache that was requested the longest time ago
//  for replacement regardless of its priority.

Define_Module_Like( LRUCache, ACache )

LRUCache::
    LRUCache( const char *namestr, cModule *parent, size_t stack ):
  AbstractCache( namestr, parent, stack )
{
}

void LRUCache::setup( void )
{
  Cache.setup( CompareBlocksTimeStamp, false );
}

bool LRUCache::checkCache(cMessage *msg_to_check)
{
  int             block_to_find;
  int             block;
  int             new_priority;
  cMessage       *msg;

  block_to_find = msg_to_check->par("block").longValue();
  new_priority  = msg_to_check->par("priority").longValue();
  ev << className() << ": Looking for (" << block_to_find << ", " <<
     new_priority << ")" << endl;

  for( cQueueIterator iter(Cache, 1); !iter.end() ; iter++ ) {
    msg = (cMessage *) iter();
    block = msg->par("block").longValue();

    // ev << className() << ": Checking " << block << " for " <<
    //       block_to_find << endl;
    if ( block_to_find == block ) {
      msg = (cMessage *) Cache.remove( msg );
      msg->setTimestamp();
      Cache.insert( msg );
      ev << className() << ": Found " << block_to_find << endl;
      if ( msg_to_check->par("is_read").boolValue() ) {
        if ( !msg->par("has_data").boolValue() )
          return false;
      } else {
        msg->par("has_data").setBoolValue( true );
        msg->par("is_dirty").setBoolValue( true );
      }
      return true;
    }
  }
  // Now replace an element
  msg = (cMessage *) Cache.pop();

  ev << className() << ": Evicting (" << msg->par("block").longValue() << ", " <<
      msg->par("priority").longValue() << ") for " << block_to_find << endl;

  // Check for Priority Inversion
  checkForPriorityInversion( msg );

  msg->setTimestamp();
  msg->par("block").setLongValue( block_to_find );
  msg->par("priority").setLongValue( new_priority );
  if ( msg_to_check->par("is_read").boolValue() ) {
    msg->par("has_data").setBoolValue( false );
    msg->par("is_dirty").setBoolValue( false );
  } else {
    msg->par("has_data").setBoolValue( true );
    msg->par("is_dirty").setBoolValue( true );
  }
  Cache.insert( msg );
  return false;
}

//------------------------------------------------
//
//  PriorityLRUCache
//
//  The Priority LRU Cache Management Algorithm chooses the oldest element
//  at the lowest priority to replace.
//
//  This algorithm is succeptible to a low priority task consuming all
//  the cache elements.  Consider the situation in which a low priority
//  task does a burst of disk activity.  It is possible for those requests
//  to purge all other elements in the cache.  The Fair Share algorithm
//  avoids this by placing a limit on the number of cache elements that
//  are available at each priority.

Define_Module_Like( PriorityLRUCache, ACache )

PriorityLRUCache::
    PriorityLRUCache( const char *namestr, cModule *parent, size_t stack ):
  AbstractCache( namestr, parent, stack )
{
}

void PriorityLRUCache::setup( void )
{
  Cache.setup( CompareBlocksLRUPriority, false );
}

// PriorityLRUCache::checkCache
//
//  If it is in the cache (regardless of priority), then return it from
//  the cache.
//
//  NOTE: A behavioral option is whether a match should elevate the
//        priority of an element.

bool PriorityLRUCache::checkCache(cMessage *msg_to_check)
{
  int             block_to_find;
  int             block;
  int             new_priority;
  cMessage       *msg;

  block_to_find = msg_to_check->par("block").longValue();
  new_priority  = msg_to_check->par("priority").longValue();
  ev << className() << ": Looking for (" << block_to_find << ", " <<
     new_priority << ")" << endl;

  for( cQueueIterator iter(Cache, 1); !iter.end() ; iter++ ) {
    msg = (cMessage *) iter();
    block = msg->par("block").longValue();

    // ev << className() << ": Checking " << block <<
    //       " for " << block_to_find << endl;
    if ( block_to_find == block ) {
      msg = (cMessage *) Cache.remove( msg );
      msg->setTimestamp();
      Cache.insert( msg );
      ev << className() << ": Found (" << block_to_find << ", " <<
             msg->par("priority").longValue() << ")" << endl;
      if ( msg_to_check->par("is_read").boolValue() ) {
        if ( !msg->par("has_data").boolValue() )
          return false;
      } else {
        msg->par("has_data").setBoolValue( true );
        msg->par("is_dirty").setBoolValue( true );
      }
      return true;
    }
  }
  // Now replace an element
  msg = (cMessage *) Cache.remove(Cache.head());

  ev << className() << ": Evicting (" << msg->par("block").longValue() <<
      ", " << msg->par("priority").longValue() << ") for " <<
      block_to_find << endl;

  // Check for Priority Inversion
  checkForPriorityInversion( msg );

  msg->setTimestamp();
  msg->par("priority").setLongValue( new_priority );
  msg->par("block").setLongValue( block_to_find );
  if ( msg_to_check->par("is_read").boolValue() ) {
    msg->par("has_data").setBoolValue( false );
    msg->par("is_dirty").setBoolValue( false );
  } else {
    msg->par("has_data").setBoolValue( true );
    msg->par("is_dirty").setBoolValue( true );
  }
  Cache.insert( msg );
  return false;
}

//------------------------------------------------
//
//  FairShareCache
//
//  The Fair Share Cache Management Algorithm manages the cache based
//  upon task priority.  There is a limit to the number of cache elements
//  that may be in use at each priority.  The distribution of elements
//  to the different priority levels is user configurable.

Define_Module_Like( FairShareCache, ACache )

FairShareCache::
    FairShareCache( const char *namestr, cModule *parent, size_t stack ):
  AbstractCache( namestr, parent, stack )
{
  Cache.setup( CompareBlocksLRUPriority, false );
}

void FairShareCache::setup( void )
{
  const char *filename = par("cache_file").stringValue();

  the_distribution.loadFromFile(filename);
}

void FairShareCache::initializeCache()
{
  cMessage *msg;
  char      msgname[32];
  int       max_elements_at_priority;
  double    share;

  number_of_elements = par("number_of_elements");

  for ( int p=1 ; p<=the_distribution.getMaximumPriority() ; p++ ) {

    share = the_distribution.getElement( p );
    max_elements_at_priority = (int) ((double)number_of_elements * share);

    if ( !max_elements_at_priority )
      continue;

    ev << className() << ": Allocating " << max_elements_at_priority <<
        " for priority " << p << ", share = " << share << endl;
    for ( int i=1 ; i<=max_elements_at_priority ; i++ ) {

      sprintf( msgname, "cache-%d", i );
      msg = new cMessage( msgname );
      msg->addPar("block").setLongValue(LONG_MAX);
      msg->addPar("priority").setLongValue(p);
      msg->addPar("is_dirty").setBoolValue(false);
      msg->setTimestamp();
      Cache.insert( msg );
    }
  }
}

// FairShareCache::checkCache
//
//  If it is in the cache (regardless of priority), then return it from
//  the cache.
//
//  NOTE: A behavioral option is whether a match should elevate the
//        priority of an element.

bool FairShareCache::checkCache(cMessage *msg_to_check)
{
  int             block_to_find;
  int             block;
  int             new_priority;
  long            this_priority;
  cMessage       *msg;
  cMessage       *tmpmsg;

  block_to_find = msg_to_check->par("block").longValue();
  new_priority  = msg_to_check->par("priority").longValue();
  ev << className() << ": Looking for (" <<
        new_priority << ", " << block_to_find << ")" << endl;

  // If there is a match, use it regardless of priority

  for( cQueueIterator iter(Cache, 1); !iter.end() ; iter++ ) {
    msg = (cMessage *) iter();
    block = msg->par("block").longValue();
    this_priority = msg->par("priority").longValue();

    // ev << className() << ": Checking " << block <<
    //       " for " << block_to_find << endl;
    if ( block_to_find == block ) {
      msg = (cMessage *) Cache.remove( msg );

      ev << className() << ": Found (" <<
         this_priority << ", " << block_to_find << ")" << endl;

      //  This is the point that priority inheritance would be implemented.
      msg->setTimestamp();
      Cache.insert( msg );
      if ( msg_to_check->par("is_read").boolValue() ) {
        if ( !msg->par("has_data").boolValue() )
          return false;
      } else {
        msg->par("has_data").setBoolValue( true );
        msg->par("is_dirty").setBoolValue( true );
      }
      return true;
    }
  }

  //
  //  The element is not in the cache.
  //

  //  If we are not supposed to cache at this priority, then skip it
  if ( !the_distribution.getElement( new_priority ) ) {
    ev << className() << ": Not caching this element" << endl;
    return false;
  }

  ev << className() << ": Looking for an element to replace" << endl;
  // Replace the oldest element at this priority
  for ( cQueueIterator iter2(Cache, 1); !iter2.end() ; iter2++ ) {
    msg = (cMessage *) iter2();
    this_priority = msg->par("priority").longValue();
    if ( this_priority == new_priority ) {
      for ( ;; ) {
        tmpmsg = (cMessage *) iter2();
        this_priority = tmpmsg->par("priority").longValue();
        if ( this_priority != new_priority )
          goto replace_one;
        msg = tmpmsg;
        iter2++;
        if ( iter2.end() )
          goto replace_one;
      }
    }
  }

replace_one:
  msg = (cMessage *) Cache.remove( msg );
  ev << className() << ": Replacing element " <<
     "(" << msg->par("priority").longValue() << ", " <<
        msg->par("block").longValue() << ") with (" <<
        new_priority << ", " << block_to_find << ")" << endl;

  msg->setTimestamp();
  msg->par("priority").setLongValue( new_priority );
  msg->par("block").setLongValue( block_to_find );
  if ( msg_to_check->par("is_read").boolValue() ) {
    msg->par("has_data").setBoolValue( false );
    msg->par("is_dirty").setBoolValue( false );
  } else {
    msg->par("has_data").setBoolValue( true );
    msg->par("is_dirty").setBoolValue( true );
  }
  Cache.insert( msg );

  return false;
}

//------------------------------------------------
//
//  PIPriorityLRUCache
//
//  The Priority LRU with Priority Inheritance Cache Management Algorithm
//  is the same as the Priority LRU algorithm except that if an element
//  is in the cache at a low priority and is subsequently accessed by
//  a higher priority task, then that element will inherit the higher
//  priority.
//
//  Just as with the basic Priority LRU algorithm, this algorithm is
//  succeptible to a low priority task consuming all the cache elements.
//  Consider the situation in which a low priority task does a burst of
//  disk activity.  It is possible for those requests to purge all other
//  elements in the cache.  The Fair Share algorithm avoids this by placing
//  a limit on the number of cache elements that are available at each
//  priority.

Define_Module_Like( PIPriorityLRUCache, ACache )

PIPriorityLRUCache::
    PIPriorityLRUCache( const char *namestr, cModule *parent, size_t stack ):
  AbstractCache( namestr, parent, stack )
{
}

void PIPriorityLRUCache::setup( void )
{
  Cache.setup( CompareBlocksLRUPriority, false );
}

// PIPriorityLRUCache::checkCache
//
//  If it is in the cache (regardless of priority), then return it from
//  the cache.
//
//  NOTE: A behavioral option is whether a match should elevate the
//        priority of an element.

bool PIPriorityLRUCache::checkCache(cMessage *msg_to_check)
{
  int             block_to_find;
  int             block;
  int             new_priority;
  int             p;
  cMessage       *msg;

  block_to_find = msg_to_check->par("block").longValue();
  new_priority  = msg_to_check->par("priority").longValue();
  ev << className() << ": Looking for (" << block_to_find << ", " <<
     new_priority << ")" << endl;

  for( cQueueIterator iter(Cache, 1); !iter.end() ; iter++ ) {
    msg = (cMessage *) iter();
    block = msg->par("block").longValue();

    // ev << className() << ": Checking " << block <<
    //       " for " << block_to_find << endl;
    if ( block_to_find == block ) {
      msg = (cMessage *) Cache.remove( msg );
      msg->setTimestamp();
      p = msg->par("priority").longValue();

      if ( p > new_priority )
        msg->par("priority").setLongValue( new_priority );

      ev << className() << ": Found (" << block_to_find << ", " <<
         msg->par("priority").longValue() << ") was at priority " << p << endl;

      Cache.insert( msg );

      if ( msg_to_check->par("is_read").boolValue() ) {
        if ( !msg->par("has_data").boolValue() )
          return false;
      } else {
        msg->par("has_data").setBoolValue( true );
        msg->par("is_dirty").setBoolValue( true );
      }
      return true;
    }
  }
  // Now replace an element
  msg = (cMessage *) Cache.remove(Cache.head());

  ev << className() << ": Evicting (" << msg->par("block").longValue() <<
      ", " << msg->par("priority").longValue() << ") for " <<
      block_to_find << endl;

  // Check for Priority Inversion
  checkForPriorityInversion( msg );

  msg->setTimestamp();
  msg->par("priority").setLongValue( new_priority );
  msg->par("block").setLongValue( block_to_find );
  if ( msg_to_check->par("is_read").boolValue() ) {
    msg->par("has_data").setBoolValue( false );
    msg->par("is_dirty").setBoolValue( false );
  } else {
    msg->par("has_data").setBoolValue( true );
    msg->par("is_dirty").setBoolValue( true );
  }
  Cache.insert( msg );
  return false;
}

//------------------------------------------------
//
//  PIFairShareCache
//
//  The Fair Share with Priority Inheritance Cache Management Algorithm
//  is the same as the Fair Share algorithm except that if an element
//  is in the cache at a low priority and is subsequently accessed by
//  a higher priority task, then that element will inherit the higher
//  priority.
//
//  The Fair Share Cache Management Algorithm manages the cache based
//  upon task priority.  There is a limit to the number of cache elements
//  that may be in use at each priority.  The distribution of elements
//  to the different priority levels is user configurable.

Define_Module_Like( PIFairShareCache, ACache )

PIFairShareCache::
    PIFairShareCache( const char *namestr, cModule *parent, size_t stack ):
  AbstractCache( namestr, parent, stack )
{
  Cache.setup( CompareBlocksLRUPriority, false );
}

void PIFairShareCache::setup( void )
{
  const char *filename = par("cache_file").stringValue();

  the_distribution.loadFromFile(filename);
}

void PIFairShareCache::initializeCache()
{
  cMessage *msg;
  char      msgname[32];
  int       max_elements_at_priority;
  double    share;

  number_of_elements = par("number_of_elements");

  for ( int p=1 ; p<=the_distribution.getMaximumPriority() ; p++ ) {

    share = the_distribution.getElement( p );
    max_elements_at_priority = (int) ((double)number_of_elements * share);

    if ( !max_elements_at_priority )
      continue;

    ev << className() << ": Allocating " << max_elements_at_priority <<
        " for priority " << p << ", share = " << share << endl;
    for ( int i=1 ; i<=max_elements_at_priority ; i++ ) {

      sprintf( msgname, "cache-%d", i );
      msg = new cMessage( msgname );
      msg->addPar("block").setLongValue( LONG_MAX );
      msg->addPar("priority").setLongValue( p );
      msg->addPar("is_dirty").setBoolValue( false );
      msg->setTimestamp();
      Cache.insert( msg );
    }
  }
}

// PIFairShareCache::checkCache
//
//  If it is in the cache (regardless of priority), then return it from
//  the cache.
//
//  NOTE: A behavioral option is whether a match should elevate the
//        priority of an element.

bool PIFairShareCache::checkCache(cMessage *msg_to_check)
{
  int             block_to_find;
  int             block;
  int             new_priority;
  long            this_priority;
  cMessage       *msg;
  cMessage       *tmpmsg;
  bool            is_hit = false;

  block_to_find = msg_to_check->par("block").longValue();
  new_priority  = msg_to_check->par("priority").longValue();
  ev << className() << ": Looking for (" <<
        new_priority << ", " << block_to_find << ")" << endl;

  // If there is a match, use it regardless of priority

  for( cQueueIterator iter(Cache, 1); !iter.end() ; iter++ ) {
    msg = (cMessage *) iter();
    block = msg->par("block").longValue();
    this_priority = msg->par("priority").longValue();

    // ev << className() << ": Checking " << block <<
    //       " for " << block_to_find << endl;
    if ( block_to_find == block ) {
      msg = (cMessage *) Cache.remove( msg );

      ev << className() << ": Found (" <<
         this_priority << ", " << block_to_find << ")" << endl;

      //  Priority Inheritance replacement occurs here

      //                        IF
      //  There is not supposed to be caching at this priority, then don't
      //  change the priority of the element such that it drops from the
      //  cache.
      //                        OR
      //  The request is at a lower priority than the element is
      //  already at, then there is nothing to inherit.
      //                       THEN
      //  Just mark it as acessed more recently.


      if ( !the_distribution.getElement( new_priority ) ||
           new_priority >= this_priority ) {
        // This saves duplicating the real code, but we need the debug info

        ev << className() << ": ";
        if ( !the_distribution.getElement( new_priority ) )
          ev << "Don't inherit priority - no elements" << endl;
        else
          ev << "Only inherit a higher priority" << endl;

        msg->setTimestamp();
        Cache.insert( msg );
        if ( msg_to_check->par("is_read").boolValue() ) {
          if ( !msg->par("has_data").boolValue() )
            return false;
        } else {
          msg->par("has_data").setBoolValue( true );
          msg->par("is_dirty").setBoolValue( true );
        }
        return true;
      }

      //  OK, we can raise the priority of this element

      //  Start by freeing the element at this priority

      ev << className() << ": Removing element at priority " <<
         new_priority << endl;
      msg->setTimestamp();
      msg->par("block").setLongValue( (long) LONG_MAX );
      msg->par("is_dirty").setBoolValue( (bool) false );
      msg->par("has_data").setBoolValue( false );
      Cache.insert( msg );

      //  Now find one to replace
      is_hit = true;
      goto find_one_to_replace;
    }
  }

  //
  //  The element is not in the cache.
  //

  //  If we are not supposed to cache at this priority, then skip it
  if ( !the_distribution.getElement( new_priority ) ) {
    ev << className() << ": Not caching this element" << endl;
    return false;
  }

find_one_to_replace:

  ev << className() << ": Looking for an element to replace" << endl;
  // Replace the oldest element at this priority
  for ( cQueueIterator iter3(Cache, 1); !iter3.end() ; iter3++ ) {
    msg = (cMessage *) iter3();
    this_priority = msg->par("priority").longValue();
    if ( this_priority == new_priority ) {
      for ( ;; ) {
        tmpmsg = (cMessage *) iter3();
        this_priority = tmpmsg->par("priority").longValue();
        if ( this_priority != new_priority )
          goto replace_one;
        msg = tmpmsg;
        iter3++;
        if ( iter3.end() )
          goto replace_one;
      }
    }
  }

  // Should never get here
  return false;

replace_one:
  msg = (cMessage *) Cache.remove( msg );
  ev << className() << ": Replacing element " <<
     "(" << msg->par("priority").longValue() << ", " <<
        msg->par("block").longValue() << ") with (" <<
        new_priority << ", " << block_to_find << ")" << endl;

  msg->setTimestamp();
  msg->par("priority").setLongValue( new_priority );
  msg->par("block").setLongValue( block_to_find );
  if ( msg_to_check->par("is_read").boolValue() ) {
    msg->par("has_data").setBoolValue( false );
    msg->par("is_dirty").setBoolValue( false );
  } else {
    msg->par("has_data").setBoolValue( true );
    msg->par("is_dirty").setBoolValue( true );
  }
  Cache.insert( msg );

  return is_hit;
}

