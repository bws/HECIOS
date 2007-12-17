#ifndef FS_WRITE_H
#define FS_WRITE_H
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
#include "basic_types.h"
class cMessage;
class FSClient;
class spfsDataFlowFinish;
class spfsMPIFileWriteAtRequest;
class spfsWriteCompletionResponse;
class spfsWriteResponse;

/**
 * Class responsible for performing client-side file writes
 */
class FSWrite
{
public:

    /** Constructor */
    FSWrite(FSClient* client, spfsMPIFileWriteAtRequest* writeReq);
    
    /** Handle MPI-Open Message */
    void handleMessage(cMessage* msg);

protected:
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

    /** Send final client response */
    void finish();
    
private:

    /** The filesystem client module */
    FSClient* client_;

    /** The originating MPI write request */
    spfsMPIFileWriteAtRequest* writeReq_;

    /** The number of bytes written */
    FSSize bytesWritten_;
};

#endif

/*
 * Local variables:
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 *
 * vim: ts=4 sts=4 sw=4 expandtab foldmethod=marker
 */
