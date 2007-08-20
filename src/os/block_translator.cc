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

#include "block_translator.h"
#include <cassert>
#include "os_proto_m.h"
using namespace std;

BlockTranslator::BlockTranslator()
{
}

void BlockTranslator::initialize()
{
    inGateId_ = gate("in")->id();
}

void BlockTranslator::handleMessage(cMessage *msg)
{
    if (msg->arrivalGateId() == inGateId_)
    {
        spfsOSReadBlocksRequest* read =
            dynamic_cast<spfsOSReadBlocksRequest*>(msg);
        assert(0 != read);

        const size_t numBlocks = read->getBlocksArraySize();
        for (size_t i = 0; i < numBlocks; i++)
        {
            long long address = getAddress(read->getBlocks(i));
            spfsOSReadDeviceRequest* req = new spfsOSReadDeviceRequest();
            req->setAddress(address);
            req->setContextPointer(msg);
            send(req, "request");
        }
    }
    else
    {
        //FIXME - doesn't do correct counting here
        cMessage* devRequest = (cMessage*)msg->contextPointer();
        cMessage* parentRequest = (cMessage*)devRequest->contextPointer();

        // Construct the correct response type
        spfsOSReadBlocksResponse* resp = new spfsOSReadBlocksResponse();
        resp->setContextPointer(parentRequest);
        send(resp, "out");

        // Clean up the device request and responses
        delete devRequest;
        delete msg;
    }
}

//------------------------------------------------
Define_Module_Like( NoTranslation, BlockTranslator )

NoTranslation::NoTranslation()
{
}

long long NoTranslation::getAddress(FSBlock block) const
{
    return block;
}

/*
 * Local variables:
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 *
 * vim: ts=8 sts=4 sw=4 expandtab
 */
