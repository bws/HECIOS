#ifndef FS_WRITE_SM_H
#define FS_WRITE_SM_H
//
// This file is part of Hecios
//
// Copyright (C) 2008 Brad Settlemyer
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
#include "fs_state_machine.h"
class cFSM;
class cMessage;
class FileDescriptor;
class FSClient;
class spfsMPIRequest;
class spfsDataFlowFinish;
class spfsMPIFileWriteAtRequest;
class spfsWriteCompletionResponse;
class spfsWriteResponse;

/**
 * Class responsible for writing a file
 */
class FSWriteSM : public FSStateMachine
{
public:
    /** Construct the file read state machine */
    FSWriteSM(spfsMPIFileWriteAtRequest* writeReq, FSClient* client);

protected:
    /** Message processing for removes */
    virtual bool updateState(cFSM& currentState, cMessage* msg);

private:
    /** Send messages establishing the write flows */
    void beginWrite();

    /** Start a flow */
    void startFlow(spfsWriteResponse* writeResponse);

    /** Count write completions */
    void countCompletion(spfsWriteCompletionResponse* completionResponse);

    /** Count a finished flow */
    void countFlowFinish(spfsDataFlowFinish* finishMsg);

    /** Count a write response */
    void countResponse();

    /** @return true if all write flows and response messages are received */
    bool isWriteComplete();

    /** The originating MPI request */
    spfsMPIFileWriteAtRequest* writeRequest_;

    /** The filesystem client module */
    FSClient* client_;

    /** The number of bytes written */
    std::size_t bytesWritten_;
};

#endif

/*
 * Local variables:
 *  indent-tabs-mode: nil
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 *
 * vim: ts=4 sts=4 sw=4 expandtab
 */
