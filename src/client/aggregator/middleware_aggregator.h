#ifndef MIDDLEWARE_AGGREGATOR_H_
#define MIDDLEWARE_AGGREGATOR_H_
//
// This file is part of Hecios
//
// Copyright (C) 2009 Brad Settlemyer
//
// This file is distributed WITHOUT ANY WARRANTY. See the file 'License.txt'
// for details on this and other legal matters.
//
#include <cstddef>
#include <map>
#include <set>
#include <vector>
#include <omnetpp.h>
#include "aggregation_io.h"
#include "direct_message_interface.h"
class Filename;
class spfsMPIFileRequest;

/**
 * An abstract model of a middleware I/O aggregator.
 */
class MiddlewareAggregator : public cSimpleModule
{
public:
    struct CollectiveOperation
    {
        std::vector<spfsMPIFileRequest*> requests;
    };

    /** */
    typedef int CollectiveId;

    /** */
    typedef std::set<AggregationIO> CollectiveMap;

    /** */
    typedef std::map<spfsMPIFileRequest*, AggregationIO> CollectiveRequestMap;

    /** Constructor */
    MiddlewareAggregator();

    /** Abstract destructor */
    virtual ~MiddlewareAggregator() = 0;

    /** Add the delay associated with copying the memory in and out of the cache */
    void addCacheMemoryDelay(cMessage* origRequest, double delay) const;

    /** @return the byte copy time */
    double byteCopyTime() const { return byteCopyTime_; };

    /** @return the appIn gate id */
    int appInGateId() const { return appInGateId_; };

    /** @return the appOut gate id */
    int appOutGateId() const { return appOutGateId_; };

    /** @return the fsIn gate id */
    int ioInGateId() const { return ioInGateId_; };

    /** @return the fsOut gate id */
    int ioOutGateId() const { return ioOutGateId_; };

    /** Return the aggregation size */
    std::size_t getAggregatorSize() const { return aggregatorSize_; };

    /** Return the world rank */
    int getRank() const { return rank_; };

    /** Set the number of processes in the aggregator */
    void setAggregatorSize(std::size_t size);

    /** Set the MPI world rank */
    void setRank(int rank) { rank_ = rank; };

    /** Send a direct message to the aggregator */
    void directMessage(cMessage* msg);

protected:
    /** Implementation of initialize */
    virtual void initialize();

    /** Implementation of finish */
    virtual void finish();

    /** Implementation of handleMessage */
    virtual void handleMessage(cMessage* msg);

    /**
     * Send the application response directly to the application
     * that originated the message.
     */
    virtual void sendApplicationResponse(double delay, cMessage* response);

    /** @return */
    CollectiveMap* createCollectiveMap();

    /** @return */
    CollectiveRequestMap* createPendingRequestMap();

private:
    /** Interface for handling messages from the application */
    virtual void handleApplicationMessage(cMessage* msg) = 0;

    /** Interface for handling messages from the file system */
    virtual void handleFileSystemMessage(cMessage* msg) = 0;

    template<class SharedResource>
    SharedResource* createSharedResource(std::map<cModule*, SharedResource*>& sharedResourceMap);

    /** @return the compute node for this model */
    cModule* findParentComputeNode() const;

    /** Number of processes in the aggregator */
    std::size_t aggregatorSize_;

    /** The time to copy a byte of data to/from the cache */
    double byteCopyTime_;

    /** Application in gate id */
    int appInGateId_;

    /** Application out gate id */
    int appOutGateId_;

    /** I/O system in gate id */
    int ioInGateId_;

    /** I/O system gate id */
    int ioOutGateId_;

    /** MPI World rank */
    int rank_;

    /** */
    static std::map<cModule*, CollectiveMap*> sharedCollectiveMap_;

    /** */
    static std::map<cModule*, CollectiveRequestMap*> sharedPendingRequestMap_;
};


#endif /*MIDDLEWARE_AGGREGATOR_H_*/

/*
 * Local variables:
 *  indent-tabs-mode: nil
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 *
 * vim: ts=8 sts=4 sw=4 expandtab
 */
