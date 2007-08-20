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

#include "access_manager.h"


AccessManager::AccessManager()
{
}

AccessManager::~AccessManager()
{
}

void AccessManager::initialize()
{
    getBlockGateId_ = gate("get_block")->id();
    nextRequestGateId_ = gate("next_request")->id();
}

void AccessManager::finish()
{
}

void AccessManager::handleMessage(cMessage *msg)
{
}

//------------------------------------------------

Define_Module_Like( SimpleAccessManager, AccessManager )

SimpleAccessManager::SimpleAccessManager()
{
}

//------------------------------------------------

Define_Module_Like( MutexAccessManager, AccessManager )

MutexAccessManager::MutexAccessManager()
{
}

void MutexAccessManager::initialize()
{
    AccessManager::initialize();
    pendingMessageQueue_.clear();
    hasIdleDisk_ = true;
}

void MutexAccessManager::finish()
{
    AccessManager::finish();
}

void MutexAccessManager::handleMessage(cMessage *msg)
{
    if ( msg->arrivalGateId() == fromSchedulerId ) {

        if (pendingMessageQueue_.empty() && hasIdleDisk_)
        {
            hasIdleDisk_ = false;
            send(msg, "get_block");
        }
        else
        {
            pendingMessageQueue_.insert(msg);
        }
    }
    else if (msg->arrivalGateId() == fromDiskId)
    {
        if (!pendingMessageQueue_.empty())
        {
            cMessage* next = (cMessage*)pendingMessageQueue_.pop();
            send(next, "get_block");
        }
        else
        {
            hasIdleDisk_ = true;
        }
    }
}


