//
// This file is part of Hecios
//
// Copyright (C) 2009 Brad Settlemyer
//
// This file is distributed WITHOUT ANY WARRANTY. See the file 'License.txt'
// for details on this and other legal matters.
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
            //cerr << "Sending Aggregate Request Kind: " << reqs[i]->kind() << endl;
            send(reqs[i], ioOutGateId());
        }
    }
    else
    {
        //cerr << "Aggregator waiting for more requests." << endl;
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
        if (msg->kind() ==SPFS_MPI_FILE_READ_AT_RESPONSE)
        {
            spfsMPIFileReadAtResponse* appResponse =
                new spfsMPIFileReadAtResponse("ViewAwareResp", SPFS_MPI_FILE_READ_AT_RESPONSE);
            appResponse->setContextPointer(appRequest);
            sendApplicationResponse(0.0, appResponse);
        }
        else
        {
            spfsMPIFileWriteAtResponse* appResponse =
                new spfsMPIFileWriteAtResponse("ViewAwareResp", SPFS_MPI_FILE_WRITE_AT_RESPONSE);
            appResponse->setContextPointer(appRequest);
            sendApplicationResponse(0.0, appResponse);
        }
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
