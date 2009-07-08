#ifndef FS_OPERATION_STATE_H
#define FS_OPERATION_STATE_H
//
// This file is part of Hecios
//
// Copyright (C) 2007,2008,2009 Brad Settlemyer
//
// This file is distributed WITHOUT ANY WARRANTY. See the file 'License.txt'
// for details on this and other legal matters.
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
