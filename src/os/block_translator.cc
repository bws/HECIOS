/**
 * @file block_translator.cc
 *
 * Note: These classes are adapted from the FSS simulator project and are
 * licensed only under the GPL.  The FSS project is available at:
 * http://www.omnetpp.org/filemgmt/singlefile.php?lid=104 and
 * http://www.omnetpp.org/doc/FSS-doc/neddoc/index.html
 */

#include <string.h>
#include <omnetpp.h>
#include "block_translator.h"


AbstractBlockTranslator::
  AbstractBlockTranslator( const char *namestr, cModule *parent, size_t stack):
cSimpleModule( namestr, parent, stack),
queue("queue")
{
}

void AbstractBlockTranslator::handleMessage(cMessage *msg)
{
  // Service the request
  Translate( msg );
  ev << className() << ": Forwarding " << msg->name() << endl;
  send( msg, "out" );
}

//------------------------------------------------

Define_Module_Like( NoTranslation, ABlockTranslator )

NoTranslation::
    NoTranslation( const char *namestr, cModule *parent, size_t stack):
AbstractBlockTranslator( namestr, parent, stack)
{
}

void NoTranslation::Translate(cMessage *msg)
{
}
