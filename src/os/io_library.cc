//
// This file is part of Hecios
//
// Copyright (C) 2006 Joel Sherrill <joel@oarcorp.com>
// Copyright (C) 2007 Brad Settlemyer
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
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
    inGateId_ = gate("in")->id();
    requestGateId_ = gate("request")->id();
    outGateId_ = gate("out")->id();
}

void IOLibrary::handleMessage( cMessage *msg )
{
    if (msg->arrivalGateId() == inGateId_)
    {
        send(msg, requestGateId_);
    }
    else
    {
        send(msg, outGateId_);
    }
}

//-------------------------------------------------
Define_Module_Like(ListIOLibrary, IOLibrary);

//------------------------------------------------
Define_Module_Like( PassThroughIOLibrary, IOLibrary )


/*
 * Local variables:
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 *
 * vim: ts=8 sts=4 sw=4 expandtab
 */
