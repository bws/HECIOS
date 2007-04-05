/**
 * @file system_call_interface.cc
 *
 * Note: These classes are adapted from the FSS simulator project and are
 * licensed only under the GPL.  The FSS project is available at:
 * http://www.omnetpp.org/filemgmt/singlefile.php?lid=104 and
 * http://www.omnetpp.org/doc/FSS-doc/neddoc/index.html
 */

#include <string.h>
#include "system_call_interface.h"


AbstractSystemCallInterface::AbstractSystemCallInterface( const char *namestr, cModule *parent, size_t stack):
cSimpleModule( namestr, parent, stack),
queue("queue")
{
}

void AbstractSystemCallInterface::initialize()
{
    //bws int fromInGateId = gate("in")->id();
}

void AbstractSystemCallInterface::handleMessage(cMessage *msg)
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

Define_Module_Like( PassThroughSystemCallInterface, ASystemCallInterface );

PassThroughSystemCallInterface::
    PassThroughSystemCallInterface(const char *namestr, cModule *parent, size_t stack):
AbstractSystemCallInterface(namestr, parent, stack)
{
}
