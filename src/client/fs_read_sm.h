#ifndef FS_READ_SM_H
#define FS_READ_SM_H
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
class FSClient;
class spfsMPIFileReadAtRequest;
class spfsDataFlowFinish;
class spfsReadRequest;

/**
 * Class responsible for removing a file
 */
class FSReadSM : public FSStateMachine
{
public:
    /** Construct the file read state machine */
    FSReadSM(spfsMPIFileReadAtRequest* mpiReq, FSClient* client);

protected:
    /** Message processing for removes */
    virtual bool updateState(cFSM& currentState, cMessage* msg);

private:
    /**  Construct server read requests */
    virtual void enterRead();

    /**
     * @return true if the read data exists in the file
     * TODO: The implementation of this function is currently naive/wrong
     */
    bool fileHasReadData(std::size_t reqBytes);

    /** Count a finished flow */
    void countFlowFinish(spfsDataFlowFinish* finishMsg);

    /** Count a read response */
    void countResponse();

    /** @return true if all read responses and finished flows are received */
    bool isReadComplete();

    /** Start a flow */
    void startFlow(spfsReadRequest* serverRequest);

    /** The originating MPI request */
    spfsMPIFileReadAtRequest* readRequest_;

    /** The file system client module */
    FSClient* client_;

    /** The number of bytes read */
    std::size_t bytesRead_;
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
