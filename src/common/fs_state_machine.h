#ifndef FS_STATE_MACHINE_H
#define FS_STATE_MACHINE_H
//
// This file is part of Hecios
//
// Copyright (C) 2007,2008,2009 Brad Settlemyer
//
// This file is distributed WITHOUT ANY WARRANTY. See the file 'License.txt'
// for details on this and other legal matters.
//
class cFSM;
class cMessage;

/**
 * A file system operation state machine
 *
 * In order to use this class, you probably want to inherit from a
 * singleton adapter rather than this class directly
 */
class FSStateMachine
{
public:
    /** Constructor */
    FSStateMachine();

    /** Destructor */
    virtual ~FSStateMachine();

    /**
     * Handle a state machine message
     *
     * @return true if the state machine has sent the final response
     */
    bool handleStateMessage(cFSM& currentState, cMessage* stateMessage);

protected:
    /**
     * Update the state machine state
     *
     * @return true if the state machine has reached completion
     */
    virtual bool updateState(cFSM& currentState,
                             cMessage* stateMessage) = 0;

private:
    /** Copy constructor hidden */
    FSStateMachine(const FSStateMachine& other);

    /** Assignment operator hidden */
    FSStateMachine& operator=(const FSStateMachine& other);
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
