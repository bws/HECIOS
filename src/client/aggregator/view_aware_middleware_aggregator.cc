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
#include <cassert>
#include <map>
#include "view_aware_access_strategy.h"
#include "middleware_aggregator.h"
#include "mpi_proto_m.h"
using namespace std;

/** Model of an aggregator that does view aware semantics */
class ViewAwareMiddlewareAggregator : public MiddlewareAggregator
{
public:
    /** Constructor */
    ViewAwareMiddlewareAggregator();

protected:
    /** Perform module initialization */
    void initialize();

private:
    /** Forward application messages to file system */
    virtual void handleApplicationMessage(cMessage* msg);

    /** Forward application messages to file system */
    virtual void handleFileSystemMessage(cMessage* msg);

    /** */
    void handleCollectiveIORequest(spfsMPIFileRequest* fileRequest);

    /** */
    void handleCollectiveIOResponse(cMessage* msg);

    CollectiveMap* currentCollective_;

    CollectiveRequestMap* pendingCollectives_;

    AggregatorAccessStrategy* aggregator_;
};

// OMNet Registration Method
Define_Module(ViewAwareMiddlewareAggregator);

void ViewAwareMiddlewareAggregator::initialize()
{
    MiddlewareAggregator::initialize();
    currentCollective_ = createCollectiveMap();
    pendingCollectives_ = createPendingRequestMap();
    aggregator_ = new ViewAwareAccessStrategy();
}

ViewAwareMiddlewareAggregator::ViewAwareMiddlewareAggregator()
{
}

// Perform simple pass through on all messages
void ViewAwareMiddlewareAggregator::handleApplicationMessage(cMessage* msg)
{
    if (SPFS_MPI_FILE_READ_AT_REQUEST == msg->kind() ||
        SPFS_MPI_FILE_WRITE_AT_REQUEST == msg->kind())
    {
        // Check if the op is collective
        spfsMPIFileRequest* fileRequest = dynamic_cast<spfsMPIFileRequest*>(msg);
        if (fileRequest->getIsCollective())
        {
            handleCollectiveIORequest(fileRequest);
        }
        else
        {
            send(msg, ioOutGateId());
        }
    }
    else
    {
        send(msg, ioOutGateId());
    }
}

void ViewAwareMiddlewareAggregator::handleFileSystemMessage(cMessage* msg)
{
    if (SPFS_MPI_FILE_READ_AT_RESPONSE == msg->kind() ||
        SPFS_MPI_FILE_WRITE_AT_RESPONSE == msg->kind())
    {
        // Check if the op was collective
        if (0 != currentCollective_->size())
        {
            handleCollectiveIOResponse(msg);
        }
        else
        {
            send(msg, appOutGateId());
        }
    }
    else
    {
        send(msg, appOutGateId());
    }
}

void ViewAwareMiddlewareAggregator::handleCollectiveIORequest(spfsMPIFileRequest* fileRequest)
{
    AggregationIO aggIO = AggregationIO::createAggregationIO(fileRequest);
    currentCollective_->insert(aggIO);
    //cerr << "Agg size: " << getAggregatorSize() << " current: " << currentCollective_->size();
    if (currentCollective_->size() == getAggregatorSize())
    {
        vector<spfsMPIFileRequest*> reqs =
            aggregator_->joinRequests(*currentCollective_);
        assert(size_t(1) == reqs.size());
        for (size_t i = 0; i < reqs.size(); i++)
        {
            send(reqs[0], ioOutGateId());
        }
    }
    else
    {
        //cerr << "Waiting bitches" << endl;
    }
}

void ViewAwareMiddlewareAggregator::handleCollectiveIOResponse(cMessage* msg)
{
    // Construct responses
    CollectiveMap::const_iterator first = currentCollective_->begin();
    CollectiveMap::const_iterator last = currentCollective_->end();
    while (first != last)
    {
        spfsMPIFileRequest* appRequest = (first++)->getRequest();
        spfsMPIFileReadAtResponse* appResponse =
            new spfsMPIFileReadAtResponse("ViewAwareResp", SPFS_MPI_FILE_READ_AT_RESPONSE);
        appResponse->setContextPointer(appRequest);
        sendApplicationResponse(0.0, appResponse);
    }

    // Cleanup the data sieving request's data
    cMessage* request = static_cast<cMessage*>(msg->contextPointer());
    spfsMPIFileRequest* fileRequest = dynamic_cast<spfsMPIFileRequest*>(request);
    FileDescriptor* fd = fileRequest->getFileDes();
    delete fd;
    delete fileRequest;
    delete msg;

    // Cleanup the current collective data
    currentCollective_->clear();
}

/*
 * Local variables:
 *  indent-tabs-mode: nil
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 *
 * vim: ts=8 sts=4 sw=4 expandtab
 */
