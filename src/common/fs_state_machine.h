#ifndef FS_STATE_MACHINE_H
#define FS_STATE_MACHINE_H
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
