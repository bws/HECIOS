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
#include <algorithm>
#include "fs_operation_state.h"
using namespace std;

FSOperationState::FSOperationState()
    : numCompletedStateMachines_(0)
{
}

FSOperationState::FSOperationState(const FSOperationState& other)
    :  numCompletedStateMachines_(other.numCompletedStateMachines_),
       currentState_(other.currentState_)
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
