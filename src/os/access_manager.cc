/**
 * @file access_manager.cc
 *
 * Note: These classes are adapted from the FSS simulator project and are
 * licensed only under the GPL.  The FSS project is available at:
 * http://www.omnetpp.org/filemgmt/singlefile.php?lid=104 and
 * http://www.omnetpp.org/doc/FSS-doc/neddoc/index.html
 */

#include <string.h>
#include <omnetpp.h>
#include "access_manager.h"


AbstractAccessManager::
    AbstractAccessManager( const char *namestr, cModule *parent, size_t stack):
cSimpleModule( namestr, parent, stack)
{
    //RequestMsg = new cMessage( "get-next-request" );
}

AbstractAccessManager::~AbstractAccessManager()
{
  // Andras .. I still get core dumps if I enable this.  Why?
  // if ( RequestMsg && !RequestMsg->isScheduled() )
  //   delete RequestMsg;
}

void AbstractAccessManager::initialize()
{
  DiskIdle = true;
  RequestPending = false;

  fromSchedulerId = gate("next_request")->id();
  fromDiskId = gate("block")->id();

  // Prime the pump by asking the Scheduler for the FIRST disk request
  //send( RequestMsg, "get_next_request" );
}

void AbstractAccessManager::finish()
{
}

void AbstractAccessManager::handleMessage(cMessage *msg)
{
  // Process the request
  if ( msg->arrivalGateId() == fromSchedulerId ) {
    // If the message came from the Scheduler, then pass it on to the Disk.
    if ( msg == RequestMsg ) {
      RequestMsg->setName( "get-next-request" );
      send( RequestMsg, "get_next_request" );
    } else {
      send( msg, "get_block" );
    }

  } else if ( msg->arrivalGateId() == fromDiskId ) {
    // If the message came from the Disk, then pass it on to the Cache.
    send( msg, "out" );
  }
}

//------------------------------------------------

Define_Module_Like( SimpleAccessManager, AAccessManager )

SimpleAccessManager::
    SimpleAccessManager( const char *namestr, cModule *parent, size_t stack):
AbstractAccessManager( namestr, parent, stack)
{
}

//------------------------------------------------

Define_Module_Like( MutexAccessManager, AAccessManager )

MutexAccessManager::
    MutexAccessManager( const char *namestr, cModule *parent, size_t stack):
AbstractAccessManager( namestr, parent, stack)
{
}

void MutexAccessManager::handleMessage(cMessage *msg)
{
  if ( msg->arrivalGateId() == fromSchedulerId ) {
    // If the message came from the Scheduler, then pass it to the Disk.
    if ( msg == RequestMsg ) {
      RequestPending = false;
    } else {
      send( msg, "get_block" );
      DiskIdle = false;
    }
  } else if ( msg->arrivalGateId() == fromDiskId ) {
    // If the message came from the Disk, then pass it on to the Cache.
    send( msg, "out" );
    DiskIdle = true;
    if ( !RequestPending ) {
      RequestMsg->setName( "get-next-request" );
      send( RequestMsg, "get_next_request" );
      RequestPending = true;
    }
  }
}


