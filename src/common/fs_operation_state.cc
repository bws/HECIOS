//
// This file is part of Hecios
//
// Copyright (C) 2007,2008,2009 Brad Settlemyer
//
// This file is distributed WITHOUT ANY WARRANTY. See the file 'License.txt'
// for details on this and other legal matters.
//
#include <algorithm>
#include "fs_operation_state.h"
using namespace std;

FSOperationState::FSOperationState()
    : numCompletedStateMachines_(0),
      numRemainingResponses_(0)
{
}

FSOperationState::FSOperationState(const FSOperationState& other)
    :  numCompletedStateMachines_(other.numCompletedStateMachines_),
       currentState_(other.currentState_),
       numRemainingResponses_(0)
{
}

FSOperationState::~FSOperationState()
{
}

FSOperationState& FSOperationState::operator=(const FSOperationState& other)
{
    // Standard swap based implementation
    FSOperationState temp(other);
    swap(temp);
    return *this;
}

void FSOperationState::swap(FSOperationState& other)
{
    std::swap(numCompletedStateMachines_, other.numCompletedStateMachines_);
    std::swap(currentState_, other.currentState_);
}

size_t FSOperationState::currentStateMachine() const
{
    return numCompletedStateMachines_;
}

cFSM* FSOperationState::currentState()
{
    return &currentState_;
}

void FSOperationState::finishCurrentStateMachine()
{
    numCompletedStateMachines_++;

    // Note: The following code makes assumptions about how cFSM is
    // implemented.  A new version of the OMNeT library may break this code
    currentState_.setState(0, 0);
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
