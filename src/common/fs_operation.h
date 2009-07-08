#ifndef FS_OPERATION_H
#define FS_OPERATION_H
//
// This file is part of Hecios
//
// Copyright (C) 2007,2008,2009 Brad Settlemyer
//
// This file is distributed WITHOUT ANY WARRANTY. See the file 'License.txt'
// for details on this and other legal matters.
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
