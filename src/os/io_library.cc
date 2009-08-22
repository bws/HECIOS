//
// This file is part of Hecios
//
// Copyright (C) 2006 Joel Sherrill <joel@oarcorp.com>
// Copyright (C) 2007,2008,2009 Brad Settlemyer
//
// This file is distributed WITHOUT ANY WARRANTY. See the file 'License.txt'
// for details on this and other legal matters.
//
#include <cassert>
#include <iostream>
#include "file_system.h"
#include "os_proto_m.h"
#include "io_library.h"
using namespace std;

void IOLibrary::initialize()
{
    // Store gate ids
    inGateId_ = findGate("in");
    requestGateId_ = findGate("request");
    outGateId_ = findGate("out");
}

void IOLibrary::handleMessage( cMessage *msg )
{
    if (msg->getArrivalGateId() == inGateId_)
    {
        send(msg, requestGateId_);
    }
    else
    {
        send(msg, outGateId_);
    }
}

//-------------------------------------------------
Define_Module(ListIOLibrary);

//------------------------------------------------
Define_Module(PassThroughIOLibrary)


/*
 * Local variables:
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 *
 * vim: ts=8 sts=4 sw=4 expandtab
 */
