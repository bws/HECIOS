//
// This file is part of Hecios
//
// Copyright (C) 2009 Bradley W. Settlemyer
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

#include "read_pages.h"
#include <algorithm>
#include <cassert>
#include <numeric>
#include <omnetpp.h>
#include "client_cache_directory.h"
#include "data_flow.h"
#include "file_distribution.h"
#include "file_page.h"
#include "file_page_utils.h"
#include "filename.h"
#include "fs_server.h"
#include "os_proto_m.h"
#include "pvfs_proto_m.h"
using namespace std;

ReadPages::ReadPages(FSServer* module, spfsReadPagesRequest* readReq)
    : module_(module),
      readReq_(readReq),
      cleanupRequest_(false)
{
}

ReadPages::~ReadPages()
{
    if (cleanupRequest_)
    {
        // TODO: Fix this leak
        //delete readReq_->getView();
        delete readReq_->getDist();
        delete readReq_;
        readReq_ = 0;
    }
}

void ReadPages::handleServerMessage(cMessage* msg)
{
    // Restore the existing state for this request
    cFSM currentState = readReq_->getState();

    // Server lookup states
    enum {
        INIT = 0,
        DETERMINE_PAGE_LOCATIONS = FSM_Transient(1),
        START_LOCAL_DATA_FLOW = FSM_Transient(2),
        SEND_FORWARD_REQUESTS = FSM_Transient(3),
        SEND_FINAL_RESPONSE = FSM_Steady(4),
        FINISH = FSM_Steady(5),
    };

    // Page locations
    size_t numClientPages = 0;
    size_t numServerPages = 0;
    vector<spfsInvalidatePagesRequest*> invalidations;

    FSM_Switch(currentState)
    {
        case FSM_Exit(INIT):
        {
            assert(0 != dynamic_cast<spfsReadPagesRequest*>(msg));
            module_->recordReadPages();

            // Create the forwarding requests
            invalidations = createInvalidatePagesRequests(numClientPages,
                                                          numServerPages);

            // Determine what further processing is required
            if (0 == numServerPages && invalidations.empty())
            {
                FSM_Goto(currentState, SEND_FINAL_RESPONSE);
            }
            else if (0 == numServerPages)
            {
                FSM_Goto(currentState, SEND_FORWARD_REQUESTS);
            }
            else
            {
                FSM_Goto(currentState, START_LOCAL_DATA_FLOW);
            }
            break;
        }
        case FSM_Enter(START_LOCAL_DATA_FLOW):
        {
            assert(0 != dynamic_cast<spfsReadRequest*>(msg));
            startDataFlow();
            break;
        }
        case FSM_Exit(START_LOCAL_DATA_FLOW):
        {
            if (invalidations.empty())
            {
                FSM_Goto(currentState, SEND_FINAL_RESPONSE);
            }
            else
            {
                FSM_Goto(currentState, SEND_FORWARD_REQUESTS);
            }
            break;
        }
        case FSM_Enter(SEND_FORWARD_REQUESTS):
        {
            assert(0 != dynamic_cast<spfsReadRequest*>(msg));
            sendInvalidateRequests(invalidations);
            break;
        }
        case FSM_Exit(SEND_FORWARD_REQUESTS):
        {
            FSM_Goto(currentState, SEND_FINAL_RESPONSE);
            break;
        }
        case FSM_Enter(SEND_FINAL_RESPONSE):
        {
            assert(0 != dynamic_cast<spfsReadRequest*>(msg));
            sendFinalResponse(numClientPages, numServerPages);
            break;
        }
        case FSM_Exit(SEND_FINAL_RESPONSE):
        {
            FSM_Goto(currentState, FINISH);
            break;
        }
        case FSM_Enter(FINISH):
        {
            assert(0 != dynamic_cast<spfsDataFlowFinish*>(msg));
            finish();
            break;
        }
    }

    // Store current state
    readReq_->setState(currentState);
}

vector<spfsInvalidatePagesRequest*>
ReadPages::createInvalidatePagesRequests(size_t& numClientPages,
                                         size_t& numServerPages)
{
    // Turn the request into pages
    FilePageUtils& pageUtils = FilePageUtils::instance();
    set<FilePageId> requestPages = pageUtils.determineRequestPages(readReq_->getPageSize(),
                                                                   readReq_->getOffset(),
                                                                   readReq_->getDataSize(),
                                                                   *readReq_->getView(),
                                                                   *readReq_->getDist());

    // Determine the invalidations based on the Request
    ClientCacheDirectory& cacheDirectory = ClientCacheDirectory::instance();
    Filename filename(readReq_->getFilename());
    ClientCacheDirectory::InvalidationMap invalidations =
        cacheDirectory.getClientsNeedingInvalidate(filename,
                                                   readReq_->getPageSize(),
                                                   readReq_->getOffset(),
                                                   readReq_->getDataSize(),
                                                   *readReq_->getView(),
                                                   *readReq_->getDist());

    // Construct the invalidation requests
    size_t exclusivePageCount = 0;
    vector<spfsInvalidatePagesRequest*> invalidateReqs;
    ClientCacheDirectory::InvalidationMap::const_iterator iter = invalidations.begin();
    ClientCacheDirectory::InvalidationMap::const_iterator last = invalidations.end();
    while (iter != last)
    {
        // Construct the request
        spfsInvalidatePagesRequest* request = new spfsInvalidatePagesRequest(0,
                                                                             SPFS_INVALIDATE_PAGES_REQUEST);
        request->setPageSize(readReq_->getPageSize());
        request->setForwardToDestination(true);
        request->setFilename(readReq_->getFilename());

        // Now add the pages to the  request
        set<ClientCacheDirectory::Entry>::const_iterator entryIter = iter->second.begin();
        set<ClientCacheDirectory::Entry>::const_iterator entryEnd = iter->second.end();
        size_t idx = 0;
        while (entryIter != entryEnd)
        {
            request->setPageIds(idx, entryIter->pageId);
            if (ClientCacheDirectory::EXCLUSIVE == entryIter->state)
            {
                exclusivePageCount++;
            }

            // Increment loop counters
            ++entryIter;
            ++idx;
        }

        // Add request to list and increment iterator
        invalidateReqs.push_back(request);
        ++iter;
    }

    // Set the outbound parameters
    numServerPages = requestPages.size() - exclusivePageCount;
    numClientPages = exclusivePageCount;
    return invalidateReqs;
}

void ReadPages::startDataFlow()
{
    // Construct the data flow start message
    spfsServerDataFlowStart* dataFlowStart =
        new spfsServerDataFlowStart(0, SPFS_DATA_FLOW_START);
    dataFlowStart->setContextPointer(readReq_);

    // Set the flow configuration
    dataFlowStart->setFlowType(DataFlow::SERVER_FLOW_TYPE);
    dataFlowStart->setFlowMode(DataFlow::READ_MODE);

    // Set the BMI connection parameters
    dataFlowStart->setBmiConnectionId(readReq_->getBmiConnectionId());
    dataFlowStart->setInboundBmiTag(readReq_->getServerFlowBmiTag());
    dataFlowStart->setOutboundBmiTag(readReq_->getClientFlowBmiTag());

    // Data transfer configuration
    dataFlowStart->setHandle(readReq_->getHandle());
    dataFlowStart->setOffset(readReq_->getOffset());
    dataFlowStart->setDataSize(readReq_->getDataSize());
    dataFlowStart->setView(readReq_->getView());
    dataFlowStart->setDist(readReq_->getDist());

    module_->send(dataFlowStart);
}

void ReadPages::sendInvalidateRequests(vector<spfsInvalidatePagesRequest*> invalidations)
{
    for (size_t i = 0; i < invalidations.size(); i++)
    {
        module_->send(invalidations[i]);
    }
}

void ReadPages::sendFinalResponse(size_t& numClientPages,
                                  size_t& numServerPages)
{
    // Construct the final response
    spfsReadPagesResponse* resp = new spfsReadPagesResponse(
        0, SPFS_READ_PAGES_RESPONSE);
    resp->setContextPointer(readReq_);
    resp->setServerPages(numServerPages);
    resp->setClientPages(numClientPages);
    //resp->setPageSize(readReq_->getPageSize());
    resp->setByteLength(4 + 4 + 4);
    module_->send(resp);
}

void ReadPages::finish()
{
    // If a flow was initiated for this request
    if (0 != readReq_->getLocalSize())
    {
        // Set the flag so that the originating request is cleaned up during
        // object destruction.  Don't simply delete it because the state is
        // updated after this call to finish()
        cleanupRequest_ = true;
    }
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
