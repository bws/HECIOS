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
#include <vector>
#include "data_sieving_access_strategy.h"
#include "file_region_set.h"
#include "middleware_aggregator.h"
#include "mpi_proto_m.h"
using namespace std;

/** Model of an aggregator that does Argonne data sieving semantics */
class DataSievingMiddlewareAggregator : public MiddlewareAggregator
{
public:
    /** Constructor */
    DataSievingMiddlewareAggregator();

protected:
    /** */
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

    AggregatorAccessStrategy* aggregator_;

    CollectiveMap* currentCollective_;

    CollectiveRequestMap* pendingCollectives_;
};

// OMNet Registration Method
Define_Module(DataSievingMiddlewareAggregator);

DataSievingMiddlewareAggregator::DataSievingMiddlewareAggregator()
{
}

void DataSievingMiddlewareAggregator::initialize()
{
    MiddlewareAggregator::initialize();
    aggregator_ = new DataSievingAccessStrategy();
    currentCollective_ = createCollectiveMap();
    pendingCollectives_ = createPendingRequestMap();
}

// Perform simple pass through on all messages
void DataSievingMiddlewareAggregator::handleApplicationMessage(cMessage* msg)
{
    if (SPFS_MPI_FILE_READ_AT_REQUEST == msg->getKind())
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

void DataSievingMiddlewareAggregator::handleFileSystemMessage(cMessage* msg)
{
    if (SPFS_MPI_FILE_READ_AT_RESPONSE == msg->getKind())
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

void DataSievingMiddlewareAggregator::handleCollectiveIORequest(spfsMPIFileRequest* fileRequest)
{
    AggregationIO aggIO = AggregationIO::createAggregationIO(fileRequest);
    currentCollective_->insert(aggIO);
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

void DataSievingMiddlewareAggregator::handleCollectiveIOResponse(cMessage* msg)
{
    // Construct responses
    CollectiveMap::const_iterator first = currentCollective_->begin();
    CollectiveMap::const_iterator last = currentCollective_->end();
    while (first != last)
    {
        spfsMPIFileRequest* appRequest = (first++)->getRequest();
        spfsMPIFileReadAtResponse* appResponse =
            new spfsMPIFileReadAtResponse("SieveResp", SPFS_MPI_FILE_READ_AT_RESPONSE);
        appResponse->setContextPointer(appRequest);
        sendApplicationResponse(0.0, appResponse);
    }

    // Cleanup the data sieving request's data
    cMessage* request = static_cast<cMessage*>(msg->getContextPointer());
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
