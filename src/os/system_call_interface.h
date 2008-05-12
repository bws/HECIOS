#ifndef SYSTEM_CALL_INTERFACE_H
#define SYSTEM_CALL_INTERFACE_H
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
#include <omnetpp.h>
#include "serial_message_scheduler.h"

/**
 * Abstract base class for system call interface models
 */
class SystemCallInterface : public cSimpleModule
{
public:
    
    /** Constructor */
    SystemCallInterface();

protected:
    
    /**
     *  This is the initialization routine for this simulation module.
     */
    virtual void initialize();

    /**
     *  This is the finalization routine for this simulation module.
     */
    virtual void handleMessage(cMessage *msg);

    int inGateId_;

    int outGateId_;

    int requestGateId_;
};

/**
 * simple System call model that passes requests through with no translation
 */
class PassThroughSystemCallInterface : public SystemCallInterface
{
  public:
    /**
     *  This is the constructor for this simulation module.
     */
    PassThroughSystemCallInterface();
};

/**
 * System call model that sequentializes requests and applies a system call
 * delay
 */
class SequentialSystemCallInterface : public SystemCallInterface
{
public:
    /**
     *  This is the constructor for this simulation module.
     */
    SequentialSystemCallInterface();

protected:    
    /** This is the initialization routine for this simulation module. */
    virtual void initialize();

    /** This is the finalization routine for this simulation module. */
    virtual void handleMessage(cMessage *msg);

private:
    /** */
    SerialMessageScheduler messageInScheduler_;

    /** */
    SerialMessageScheduler messageOutScheduler_;
    
    /** System call overhead */
    double overheadSecs_;

};

#endif

/*
 * Local variables:
 *  indent-tabs-mode: nil
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 *
 * vim: ts=8 sts=4 sw=4 expandtab
 */
