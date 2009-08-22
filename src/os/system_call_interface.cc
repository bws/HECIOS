//
// This file is part of Hecios
//
// Copyright (C) 2006 Joel Sherrill <joel@oarcorp.com>
// Copyright (C) 2007,2008,2009 Brad Settlemyer
//
// This file is distributed WITHOUT ANY WARRANTY. See the file 'License.txt'
// for details on this and other legal matters.
//
#include "system_call_interface.h"
#include <cassert>
using namespace std;

SystemCallInterface::SystemCallInterface()
{
}

void SystemCallInterface::initialize()
{
    inGateId_ = findGate("in");
    requestGateId_ = findGate("request");
    outGateId_ = findGate("out");
}

void SystemCallInterface::handleMessage(cMessage *msg)
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

//------------------------------------------------
Define_Module(PassThroughSystemCallInterface);

PassThroughSystemCallInterface::PassThroughSystemCallInterface()
    : SystemCallInterface()
{
}

//------------------------------------------------
Define_Module(SequentialSystemCallInterface);

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
    if (msg->getArrivalGateId() == inGateId_)
    {
        // Sequence the message and add system call delay
        double overhead = overheadSecs_;
        if (SPFS_OS_FILE_READ_REQUEST == msg->getKind())
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
