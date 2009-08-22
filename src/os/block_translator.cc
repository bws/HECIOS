//
// This file is part of Hecios
//
// Copyright (C) 2007,2008,2009 Brad Settlemyer
//
// This file is distributed WITHOUT ANY WARRANTY. See the file 'License.txt'
// for details on this and other legal matters.
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
    inGateId_ = gate("in")->getId();

    // Initialize derived translators
    initializeTranslator();
}

void BlockTranslator::handleMessage(cMessage *msg)
{
    if (msg->getArrivalGateId() == inGateId_)
    {
        spfsOSBlockIORequest* blockIO = 0;
        spfsOSReadBlocksRequest* blockRead = 0;
        spfsOSWriteBlocksRequest* blockWrite = 0;

        if ((blockRead = dynamic_cast<spfsOSReadBlocksRequest*>(msg)))
        {
            blockIO = blockRead;
        }
        else if ((blockWrite = dynamic_cast<spfsOSWriteBlocksRequest*>(msg)))
        {
            blockIO = blockWrite;
        }
        assert(0 != blockRead || 0 != blockWrite);
        assert(0 != blockIO);

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
                if (0 != blockRead)
                {
                    req = new spfsOSReadDeviceRequest();
                }
                else
                {
                    assert(0 != blockWrite);
                    spfsOSWriteDeviceRequest* writeDev =
                        new spfsOSWriteDeviceRequest();
                    writeDev->setWriteThrough(blockWrite->getWriteThrough());
                    req = writeDev;
                }
                assert(0 != req);
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
        cMessage* devRequest = (cMessage*)msg->getContextPointer();

        // Extract the parent block io request
        cMessage* parentRequest = (cMessage*)devRequest->getContextPointer();
        spfsOSBlockIORequest* ioRequest =
            dynamic_cast<spfsOSBlockIORequest*>(parentRequest);
        assert(0 != ioRequest);

        // If this is the last response for this request, send a response
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
// NoTranslation implementation (abstract class)
//
//=============================================================================
Define_Module(NoTranslation);

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
Define_Module(BasicTranslator);

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
