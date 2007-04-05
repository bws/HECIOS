/**
 * @file file_system.cc
 *
 * Note: These classes are adapted from the FSS simulator project and are
 * licensed only under the GPL.  The FSS project is available at:
 * http://www.omnetpp.org/filemgmt/singlefile.php?lid=104 and
 * http://www.omnetpp.org/doc/FSS-doc/neddoc/index.html
 */

#include <string.h>
#include "file_system.h"

AbstractFileSystem::AbstractFileSystem( const char *namestr, cModule *parent, size_t stack):
cSimpleModule( namestr, parent, stack),
queue("queue")
{
}

void AbstractFileSystem::initialize()
{
  fromInGateId = gate("in")->id();
}

void AbstractFileSystem::finish()
{
}

void AbstractFileSystem::handleMessage(cMessage *msg)
{
  if ( msg->arrivalGateId()==fromInGateId ) {
    ev << className() << ": Sending " << msg->name() << " to request" << endl;
    send( msg, "request" );
  } else {
    ev << className() << ": Completed service of " << msg->name() << endl;
    send( msg, "out" );
  }
}

//------------------------------------------------

Define_Module_Like( PassThroughFileSystem, AFileSystem )

PassThroughFileSystem::
    PassThroughFileSystem(const char *namestr, cModule *parent, size_t stack):
AbstractFileSystem(namestr, parent, stack)
{
}
