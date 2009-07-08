//
// This file is part of Hecios
//
// Copyright (C) 2007,2008,2009 Brad Settlemyer
//
// This file is distributed WITHOUT ANY WARRANTY. See the file 'License.txt'
// for details on this and other legal matters.
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
