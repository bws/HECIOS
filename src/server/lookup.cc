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

#include "lookup.h"
#include <cassert>
#include <omnetpp.h>
#include "fs_server.h"
#include "pvfs_proto_m.h"
using namespace std;

Lookup::Lookup(FSServer* module, spfsLookupPathRequest* lookupReq)
    : module_(module),
      lookupReq_(lookupReq)
{
}

void Lookup::handleServerMessage(cMessage* msg)
{
    // Restore the existing state for this request
    cFSM currentState = lookupReq_->getState();

    // Server lookup states
    enum {
        INIT = 0,
        FINISH = FSM_Steady(1),
    };

    FSM_Switch(currentState)
    {
        case FSM_Enter(INIT):
        {
            assert(0 != dynamic_cast<spfsLookupPathRequest*>(msg));
            enterFinish();
            break;
        }
    }
}

void Lookup::enterFinish()
{
    spfsLookupPathResponse* resp = new spfsLookupPathResponse(
        0, SPFS_LOOKUP_PATH_RESPONSE);
    resp->setStatus(SPFS_FOUND);
    resp->setHandleCount(1);
    resp->setAttrCount(0);
    resp->setHandlesArraySize(1);
    resp->setHandles(0, 1200);
    resp->setContextPointer(lookupReq_);
    module_->send(resp, "netOut");
}

/*
 * Local variables:
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 *
 * vim: ts=4 sts=4 sw=4 expandtab foldmethod=marker
 */
