#ifndef FS_OPERATION_H
#define FS_OPERATION_H
//
// This file is part of Hecios
//
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
#include <vector>
#include "fs_operation_state.h"
class cFSM;
class cMessage;
class FSStateMachine;
/**
 * A file system operation implemented as a series of state machines
 */
class FSOperation
{
public:
   /** Constructor */
    FSOperation(FSOperationState& operationState);

    /** Destructor */
    virtual ~FSOperation();

    /** @return true if the operation is complete */
    bool isComplete() const;

    /** @return a copy of the operation's state */
    FSOperationState state() const { return operationState_; };

    /**
     * Register the state machines in the order to perform the operation
     *  fsStateMachine's memory is now owned by FSOperation
     *
     * @return the state machine's index
     */
    int addStateMachine(FSStateMachine* fsStateMachine);

    /** Perform the message processing for this operation */
    void processMessage(cMessage* operationMessage);

protected:
    /** Register the state machines required to implement this operation */
    virtual void registerStateMachines() = 0;

    /** Send the final response once all state machines have finished*/
    virtual void sendFinalResponse() = 0;

private:
    /** Copy constructor hidden */
    FSOperation(const FSOperation& other);

    /** Assignment operator hidden */
    FSOperation& operator=(const FSOperation& other);

    /** The operation state */
    FSOperationState operationState_;

    /** The state machines for this operation */
    std::vector<FSStateMachine*> stateMachines_;

    /** Flag indicating if the state machines are registered */
    bool registrationIsComplete_;
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
