#ifndef FS_OPERATION_STATE_H
#define FS_OPERATION_STATE_H
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
#include <cstddef>
#include <omnetpp.h>

/**
 * A file system operation state
 */
class FSOperationState
{
public:
    /** Constructor */
    FSOperationState();

    /** Copy constructor */
    FSOperationState(const FSOperationState& other);

    /** Destructor */
    ~FSOperationState();

    /** Assignment operator */
    FSOperationState& operator=(const FSOperationState& other);

    /** @return the index of the current operation state machine */
    std::size_t currentStateMachine() const;

    /** @return the current state of the current state machine */
    cFSM* currentState();

    /** Finish the current state machine */
    void finishCurrentStateMachine();

private:
    /** Swap the contents of this and other */
    void swap(FSOperationState& other);

    /** The number of state machines completed*/
    int numCompletedStateMachines_;

    /** The most recent state */
    cFSM currentState_;

    /** The number of responses remaining */
    std::size_t numRemainingResponses_;
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
