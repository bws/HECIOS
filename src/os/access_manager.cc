//
// This file is part of Hecios
//
// Copyright (C) 2006 Joel Sherrill <joel@oarcorp.com>
// Copyright (C) 2007,2008,2009 Brad Settlemyer
//
// This file is distributed WITHOUT ANY WARRANTY. See the file 'License.txt'
// for details on this and other legal matters.
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


