#ifndef ACCESS_MANAGER_H
#define ACCESS_MANAGER_H
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

/**
 * Abstract base class for single-server queues
 */
class AccessManager : public cSimpleModule
{
public:
    /**
     *  This is the constructor for this simulation module.
     */
    AccessManager();

    /**
     *  This is the destructor for this simulation module.
     */
    ~AccessManager();

    /**
     *  This is the initialization routine for this simulation module.
     */
    virtual void initialize();

    /**
     *  This is the finalization routine for this simulation module.
     */
    virtual void finish();

    /**
     *  This is the message handling routine for this simulation module.
     *
     *  @param msg (in) is the new message to process
     */
    virtual void handleMessage(cMessage *msg);

protected:
    int fromSchedulerId;
    int fromDiskId;

    int getNextRequestGateId_;

    int nextRequestGateId_;

    int getBlockGateId_;

    int blockGateId_;
};

/**
 * AccessManager that passes through all requests
 */
class SimpleAccessManager : public AccessManager
{
  public:
    /**
     *  This is the constructor for this simulation module.
     */
    SimpleAccessManager();
};

/**
 * AccessManager that allows only one outstanding request at a time
 */
class MutexAccessManager : public AccessManager
{
  public:
    /**
     *  This is the constructor for this simulation module.
     */
    MutexAccessManager();

protected:
    
    virtual void initialize();

    virtual void finish();

    virtual void handleMessage( cMessage *msg );

private:

    bool hasIdleDisk_;
    cQueue pendingMessageQueue_;
};

#endif
