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

//=============================================================================
//
// BlockTranslator implementation (bastract class)
//
//=============================================================================
BlockTranslator::BlockTranslator()
{
}

void BlockTranslator::initialize()
{
    inGateId_ = gate("in")->id();

    // Initialize derived translators
    initializeTranslator();
}

void BlockTranslator::handleMessage(cMessage *msg)
{
    if (msg->arrivalGateId() == inGateId_)
    {
        spfsOSBlockIORequest* blockIO = dynamic_cast<spfsOSBlockIORequest*>(msg);
        assert(0 != read);

        // Determine if we are building read or write requests
        bool isRead = (0 != dynamic_cast<spfsOSReadBlocksRequest*>(blockIO));

        // Generate requests for each block
        size_t numRequestsGenerated = 0;
        size_t numBlocks = blockIO->getBlocksArraySize();
        for (size_t i = 0; i < numBlocks; i++)
        {
            // Create multiple device requests for each block
            vector<LogicalBlockAddress> lbas =
                getAddresses(blockIO->getBlocks(i));
            for (size_t j = 0; j < lbas.size(); j++)
            {
                spfsOSDeviceIORequest* req = 0;
                if (isRead)
                {
                    req = new spfsOSReadDeviceRequest();
                }
                else
                {
                    req = new spfsOSWriteDeviceRequest();
                }
                req->setAddress(lbas[j]);
                req->setContextPointer(msg);
                send(req, "request");
            }

            // Track the number of requests generated
            numRequestsGenerated += lbas.size();
        }

        // Update the originating request with the number of requests created
        blockIO->setNumRemainingResponses(numRequestsGenerated);
    }
    else
    {
        // Extract the originating device request
        cMessage* devRequest = (cMessage*)msg->contextPointer();

        // Extract the parent block io request
        cMessage* parentRequest = (cMessage*)devRequest->contextPointer();
        spfsOSBlockIORequest* ioRequest =
            dynamic_cast<spfsOSBlockIORequest*>(parentRequest);
        assert(0 != ioRequest);

        // If this is the last response for this request, send a respnse
        // otherwise, decrement the number of remaining responses
        int numRemainingResponses = ioRequest->getNumRemainingResponses();
        if (1 == numRemainingResponses)
        {
            // Construct the correct response type
            spfsOSReadBlocksResponse* resp = new spfsOSReadBlocksResponse();
            resp->setContextPointer(ioRequest);
            send(resp, "out");
        }
        else
        {
            ioRequest->setNumRemainingResponses(numRemainingResponses - 1);
        }
        
        // Clean up the device request and response
        delete devRequest;
        delete msg;
    }
}

//=============================================================================
//
// NoTranslation implementation (bastract class)
//
//=============================================================================
Define_Module_Like(NoTranslation, BlockTranslator);

NoTranslation::NoTranslation()
{
}

vector<LogicalBlockAddress> NoTranslation::getAddresses(FSBlock block) const
{
    vector<LogicalBlockAddress> lba(1);
    lba.push_back(block);
    return lba;
}

//=============================================================================
//
// BasicTranslator implementation (bastract class)
//
//=============================================================================
Define_Module_Like(BasicTranslator, BlockTranslator);

BasicTranslator::BasicTranslator()
    : addrsPerBlock_(0)
{
}

void BasicTranslator::initializeTranslator()
{
    addrsPerBlock_ = 4096 / 512;
}

vector<LogicalBlockAddress> BasicTranslator::getAddresses(FSBlock block) const
{
    // Find the first address for this block
    LogicalBlockAddress begAddr = block * addrsPerBlock_;

    // Construct all the addresses for this block
    vector<LogicalBlockAddress> addresses;
    for (int i = 0; i < addrsPerBlock_; i++)
    {
        addresses.push_back(begAddr + i);
    }
    return addresses;
}

/*
 * Local variables:
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 *
 * vim: ts=8 sts=4 sw=4 expandtab
 */
