//
// This file is part of Hecios
//
// Copyright (C) 2007,2008,2009 Brad Settlemyer
//
// This file is distributed WITHOUT ANY WARRANTY. See the file 'License.txt'
// for details on this and other legal matters.
//
//#define FSM_DEBUG  // Enable FSM Debug output
#include "fs_cache_read_sm.h"
#include <cassert>
#include <omnetpp.h>
#include "cache_proto_m.h"
#include "data_flow.h"
#include "data_type_layout.h"
#include "data_type_processor.h"
#include "file_distribution.h"
#include "fs_client.h"
#include "pvfs_proto_m.h"
using namespace std;

FSCacheReadSM::FSCacheReadSM(spfsCacheReadRequest* readRequest,
                             FSClient* client,
                             bool isExclusive)
    : readRequest_(readRequest),
      client_(client),
      isExclusive_(isExclusive),
      bytesRead_(0)
{
    assert(0 != readRequest_);
    assert(0 != client_);
}

bool FSCacheReadSM::updateState(cFSM& currentState, cMessage* msg)
{
    /** File system write state machine states */
    enum {
        INIT = 0,
        READ = FSM_Transient(1),
        COUNT = FSM_Steady(2),
        COUNT_SERVER_RESPONSE = FSM_Transient(2),
        COUNT_FLOW_FINISH = FSM_Transient(3),
        FINISH = FSM_Steady(4),
    };

    bool isComplete = false;
    FSM_Switch(currentState)
    {
        case FSM_Exit(INIT):
        {
            FSM_Goto(currentState, READ);
            break;
        }
        case FSM_Enter(READ):
        {
            enterRead();
            break;
        }
        case FSM_Exit(READ):
        {
            if (isReadComplete())
            {
                FSM_Goto(currentState, FINISH);
            }
            else
            {
                FSM_Goto(currentState, COUNT);
            }
            break;
        }
        case FSM_Exit(COUNT):
        {
            if (SPFS_READ_PAGES_RESPONSE == msg->getKind())
            {
                FSM_Goto(currentState, COUNT_SERVER_RESPONSE);
            }
            else
            {
                assert(SPFS_DATA_FLOW_FINISH == msg->getKind());
                FSM_Goto(currentState, COUNT_FLOW_FINISH);
            }
            break;
        }
        case FSM_Enter(COUNT_SERVER_RESPONSE):
        {
            assert(0 != dynamic_cast<spfsReadPagesResponse*>(msg));

            // Need a quick workaround in case there isn't a need for a
            // flow here
            spfsReadRequest* readRequest =
                static_cast<spfsReadRequest*>(msg->getContextPointer());
            if (0 != readRequest->getLocalSize())
            {
                startFlow(dynamic_cast<spfsReadPagesResponse*>(msg));
            }
            break;
        }
        case FSM_Exit(COUNT_SERVER_RESPONSE):
        {
            spfsReadPagesResponse* response = dynamic_cast<spfsReadPagesResponse*>(msg);
            assert(0 != response);
            countResponse(response);

            if (isReadComplete())
            {
                FSM_Goto(currentState, FINISH);
            }
            else
            {
                FSM_Goto(currentState, COUNT);
            }
            break;
        }
        case FSM_Exit(COUNT_FLOW_FINISH):
        {
            countFlowFinish(dynamic_cast<spfsDataFlowFinish*>(msg));
            if (isReadComplete())
            {
                FSM_Goto(currentState, FINISH);
            }
            else
            {
                FSM_Goto(currentState, COUNT);
            }
            break;
        }
        case FSM_Enter(FINISH):
        {
            isComplete = true;
            break;
        }
    }

    return isComplete;
}

void FSCacheReadSM::enterRead()
{
    FileDescriptor* fd = readRequest_->getFileDescriptor();
    assert(0 != fd);
    const FSMetaData* metaData = fd->getMetaData();

    // Construct the template read request
    spfsReadPagesRequest read("ReadStuff", SPFS_READ_PAGES_REQUEST);

    // Note: The read request is NOT deleted here as one would expect.
    // Because the server side flow completes after the client, the request
    // is deleted on the server
    read.setMetaHandle(metaData->handle);
    read.setContextPointer(readRequest_);
    read.setOffset(readRequest_->getOffset());
    read.setView(new FileView(fd->getFileView()));
    read.setIsExclusive(isExclusive_);
    read.setPageSize(readRequest_->getPageSize());

    // Send request to each server
    int numRequests = 0;
    int numFlows = 0;
    int numServers = metaData->dataHandles.size();
    for (int i = 0; i < numServers; i++)
    {
        // Process the data to determine if any data could be on this server
        // (must use the client write processor to ensure the stream size won't
        //  limit the data distribution)
        metaData->dist->setObjectIdx(i);
        FSSize aggregateSize = 0;
        FSSize serverBytes = DataTypeProcessor::createClientFileLayoutForWrite(
            readRequest_->getOffset(),
            *readRequest_->getDataType(),
            readRequest_->getCount(),
            *read.getView(),
            *metaData->dist,
            aggregateSize);

        if (0 != serverBytes)
        {
            // Now reprocess the data using the client read functionality
            // to figure out the actual data to send to server
            FSSize reqBytes = DataTypeProcessor::createClientFileLayoutForRead(
                readRequest_->getOffset(),
                *readRequest_->getDataType(),
                readRequest_->getCount(),
                *read.getView(),
                *metaData->dist,
                metaData->bstreamSizes[i],
                aggregateSize);

            // Create a request that the server will not flow data for
            // because the read data is not present in the file
            spfsReadRequest* req = static_cast<spfsReadRequest*>(read.dup());
            req->setAutoCleanup(true);
            req->setHandle(metaData->dataHandles[i]);
            req->setOffset(readRequest_->getOffset());
            req->setDataSize(aggregateSize);
            req->setBstreamSize(metaData->bstreamSizes[i]);
            req->setLocalSize(reqBytes);

            // Add to the number of requests sent
            numRequests++;
            if (0 != reqBytes)
            {
                req->setAutoCleanup(false);
                req->setDist(metaData->dist->clone());
                req->setClientFlowBmiTag(simulation.getUniqueNumber());
                req->setServerFlowBmiTag(simulation.getUniqueNumber());
                numFlows++;
            }

            // Set the message size in bytes
            req->setByteLength(8 + 8 + 4 + 4 + 4 +
                               //FSClient::CREDENTIALS_SIZE +
                               fd->getFileView().getRepresentationByteLength() +
                               8 + 8);
            client_->send(req, client_->getNetOutGate());
        }
    }

    // Set the number of outstanding responses and flows
    readRequest_->setRemainingResponses(numRequests);
    readRequest_->setRemainingFlows(numFlows);
}

bool FSCacheReadSM::fileHasReadData(size_t reqBytes)
{
    // TODO: This is not a correct implementation, but it works for now
    // Extract the file descriptor
    FileDescriptor* fd = readRequest_->getFileDescriptor();
    assert(0 != fd);
    FSSize fileSize = fd->getMetaData()->size;
    return (reqBytes <= fileSize);
}

void FSCacheReadSM::startFlow(spfsReadPagesResponse* readResponse)
{
    size_t numPages = readResponse->getServerPageIdsArraySize();
    FSSize pageSize = readRequest_->getPageSize();
    assert(0 < numPages);
    assert(0 < pageSize);

    // Extract the file descriptor
    FileDescriptor* fd = readRequest_->getFileDescriptor();
    assert(0 != fd);

    // Extract the server request
    spfsReadPagesRequest* serverRequest =
        static_cast<spfsReadPagesRequest*>(readResponse->getContextPointer());

    // Create the flow start message
    spfsCacheDataFlowStart* flowStart = new spfsCacheDataFlowStart(0,
                                                                   SPFS_DATA_FLOW_START);
    flowStart->setContextPointer(readRequest_);
    flowStart->setClientContextPointer(serverRequest);
    flowStart->setBstreamSize(serverRequest->getBstreamSize());

    // Set the handle as the connection id (TODO: This is hacky)
    flowStart->setBmiConnectionId(serverRequest->getHandle());
    flowStart->setInboundBmiTag(serverRequest->getClientFlowBmiTag());
    flowStart->setOutboundBmiTag(serverRequest->getServerFlowBmiTag());

    // Flow configuration
    flowStart->setFlowType(DataFlow::CACHE_FLOW_TYPE);
    flowStart->setFlowMode(DataFlow::READ_MODE);

    // Data transfer configuration
    flowStart->setHandle(serverRequest->getHandle());
    flowStart->setNumPages(numPages);
    flowStart->setPageSize(pageSize);

    // Send the start message
    client_->send(flowStart, client_->getNetOutGate());
}

void FSCacheReadSM::countResponse(spfsReadPagesResponse* response)
{
    // Increment the number of responses
    int numRemainingResponses = readRequest_->getRemainingResponses() - 1;
    readRequest_->setRemainingResponses(numRemainingResponses);

    // Add the client pages to the initial request
    size_t origSize = readRequest_->getResponseCachePageIdsArraySize();
    size_t newSize = origSize + response->getClientPageIdsArraySize();
    readRequest_->setResponseCachePageIdsArraySize(newSize);
    for (size_t i = origSize, j = 0; i < newSize; i++, j++)
    {
        readRequest_->setResponseCachePageIds(i, response->getClientPageIds(j));
    }

    // Add the server pages to the initial request
    origSize = readRequest_->getResponseServerPageIdsArraySize();
    newSize = origSize + response->getServerPageIdsArraySize();
    readRequest_->setResponseServerPageIdsArraySize(newSize);
    for (size_t i = origSize, j = 0; i < newSize; i++, j++)
    {
        readRequest_->setResponseServerPageIds(i, response->getServerPageIds(j));
    }
}

void FSCacheReadSM::countFlowFinish(spfsDataFlowFinish* finishMsg)
{
    assert(0 != finishMsg);
    bytesRead_ += finishMsg->getFlowSize();
    int numRemainingFlows = readRequest_->getRemainingFlows();
    readRequest_->setRemainingFlows(--numRemainingFlows);
}

bool FSCacheReadSM::isReadComplete()
{
    return (0 == readRequest_->getRemainingResponses())
        && (0 == readRequest_->getRemainingFlows());
}

/*
 * Local variables:
 *  indent-tabs-mode: nil
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 *
 * vim: ts=4 sts=4 sw=4 expandtab
 */
