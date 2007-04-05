/**
 * @file io_library.cc
 *
 * Note: These classes are adapted from the FSS simulator project and are
 * licensed only under the GPL.  The FSS project is available at:
 * http://www.omnetpp.org/filemgmt/singlefile.php?lid=104 and
 * http://www.omnetpp.org/doc/FSS-doc/neddoc/index.html
 */

#include <string.h>
#include "io_library.h"


AbstractIOLibrary::AbstractIOLibrary( const char *namestr, cModule *parent, size_t stack):
cSimpleModule( namestr, parent, stack),
queue("queue")
{
}

void AbstractIOLibrary::initialize()
{
  fromInGateId = gate("in")->id();
}

void AbstractIOLibrary::handleMessage( cMessage *msg )
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

Define_Module_Like( PassThroughIOLibrary, AIOLibrary )

PassThroughIOLibrary::
    PassThroughIOLibrary(const char *namestr, cModule *parent, size_t stack):
AbstractIOLibrary(namestr, parent, stack)
{
}
