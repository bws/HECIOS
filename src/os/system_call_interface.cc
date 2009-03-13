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
}

void SystemCallInterface::handleMessage(cMessage *msg)
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

//------------------------------------------------
Define_Module_Like( PassThroughSystemCallInterface, SystemCallInterface );

PassThroughSystemCallInterface::PassThroughSystemCallInterface()
    : SystemCallInterface()
{
}

//------------------------------------------------
Define_Module_Like(SequentialSystemCallInterface, SystemCallInterface);

SequentialSystemCallInterface::SequentialSystemCallInterface()
    : SystemCallInterface(),
      messageInScheduler_(this),
      messageOutScheduler_(this),
      overheadSecs_(-1.0)
{
}

void SequentialSystemCallInterface::initialize()
{
    // Initialize parent
    SystemCallInterface::initialize();

    // Initialize self
    overheadSecs_ = par("overheadSecs").doubleValue();
    addReadOverheadSecs_ = par("addReadOverheadSecs").doubleValue();
    addWriteOverheadSecs_ = par("addWriteOverheadSecs").doubleValue();
    assert(0.0 <= overheadSecs_);
}

void SequentialSystemCallInterface::handleMessage(cMessage* msg)
{
    if (msg->arrivalGateId() == inGateId_)
    {
        // Sequence the message and add system call delay
        double overhead = overheadSecs_;
        if (SPFS_OS_FILE_READ_REQUEST == msg->kind())
        {
            overhead += addReadOverheadSecs_;
        }
        else if (SPFS_OS_FILE_WRITE_REQUEST)
        {
            overhead += addWriteOverheadSecs_;
        }

        simtime_t scheduleDelay =
            messageInScheduler_.getNextMessageScheduleDelay(overhead);
        sendDelayed(msg, scheduleDelay, requestGateId_);
    }
    else
    {
        // Sequence the message and add system call delay
        simtime_t scheduleDelay =
            messageInScheduler_.getNextMessageScheduleDelay(overheadSecs_);
        sendDelayed(msg, scheduleDelay, outGateId_);
    }
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
