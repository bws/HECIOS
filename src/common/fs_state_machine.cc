//
// This file is part of Hecios
//
// Copyright (C) 2007,2008,2009 Brad Settlemyer
//
// This file is distributed WITHOUT ANY WARRANTY. See the file 'License.txt'
// for details on this and other legal matters.
//
#include "fs_state_machine.h"
#include <omnetpp.h>

FSStateMachine::FSStateMachine()
{
}

FSStateMachine::~FSStateMachine()
{
}

bool FSStateMachine::handleStateMessage(cFSM& currentState,
                                        cMessage* stateMessage)
{
    return updateState(currentState, stateMessage);
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
