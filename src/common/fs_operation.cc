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
#include "fs_operation.h"
#include <cassert>
#include <omnetpp.h>
#include "fs_operation_state.h"
#include "fs_state_machine.h"
using namespace std;

FSOperation::FSOperation(FSOperationState& operationState)
    : operationState_(operationState),
      registrationIsComplete_(false)
{
}

FSOperation::~FSOperation()
{
    // Cleanup the state machines
    for (size_t i = 0; i < stateMachines_.size(); i++)
    {
        delete stateMachines_[i];
    }
}

bool FSOperation::isComplete() const
{
    return (operationState_.currentStateMachine() >= stateMachines_.size());
}

int FSOperation::addStateMachine(FSStateMachine* fsStateMachine)
{
    //cerr << __FILE__ << ":" << __LINE__ << ":" << "Adding state machine\n";
    stateMachines_.push_back(fsStateMachine);
    return stateMachines_.size() - 1;
}

void FSOperation::processMessage(cMessage* operationMessage)
{
    // Register the derived op's state machines
    if (!registrationIsComplete_)
    {
        registerStateMachines();
        registrationIsComplete_ = true;
    }

    // If there are remaining state machines, execute those
    // Else, send the final response
    if (!isComplete())
    {
        assert(operationState_.currentStateMachine() < stateMachines_.size());

        // Perform the next state
        size_t fsmIdx = operationState_.currentStateMachine();
        FSStateMachine* fsm = stateMachines_[fsmIdx];
        bool isFinished =
            fsm->handleStateMessage(*operationState_.currentState(),
                                    operationMessage);

        // Mark the current state machine finished if necessary
        if (isFinished)
        {
            //cerr << __FILE__ << ":" << __LINE__ << ":"
            //     << "Finished state machine: " << fsmIdx << endl;
            operationState_.finishCurrentStateMachine();

            // If a state machine finished, use that message on the next state
            // machine
            processMessage(operationMessage);
        }
    }
    else
    {
        //cerr << __FILE__ << ":" << __LINE__ << ":"
        //     << "Sending final response" << endl;
        sendFinalResponse();
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
