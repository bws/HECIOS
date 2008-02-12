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
#include "system_call_interface.h"
#include <cassert>
using namespace std;

SystemCallInterface::SystemCallInterface()
{
}

void SystemCallInterface::initialize()
{
    inGateId_ = gate("in")->id();
    requestGateId_ = gate("request")->id();
    outGateId_ = gate("out")->id();

    overheadSecs_ = par("overheadSecs").doubleValue();
    assert(0.0 <= overheadSecs_);
}

void SystemCallInterface::handleMessage(cMessage *msg)
{
    if (msg->isSelfMessage())
    {
        send(msg, requestGateId_);
    }
    else if (msg->arrivalGateId() == inGateId_)
    {
        scheduleAt(simTime() + overheadSecs_, msg);
    }
    else
    {
        send(msg, outGateId_);
    }
}

//------------------------------------------------

Define_Module_Like( PassThroughSystemCallInterface, SystemCallInterface );

PassThroughSystemCallInterface::PassThroughSystemCallInterface()
{
}

/*
 * Local variables:
 *  indent-tabs-mode: nil
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 *
 * vim: ts=8 sts=4 sw=4 expandtab
 */
