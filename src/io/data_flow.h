#ifndef DATA_FLOW_H
#define DATA_FLOW_H
//
// This file is part of Hecios
//
// Copyright (C) 2007,2008,2009 Brad Settlemyer
//
// This file is distributed WITHOUT ANY WARRANTY. See the file 'License.txt'
// for details on this and other legal matters.
//
#include <cstddef>
#include <omnetpp.h>
#include "basic_types.h"
#include "data_type_layout.h"
class cMessage;
class cSimpleModule;
class spfsDataFlowStart;

/**
 * A data flow for reading or writing data to storage
 *
 * NOTE: If this simulator is to be run in parallel, flows will require
 * some form of synchronization, particularly on the bytesProcessed
 * variable
 */
class DataFlow
{
public:
    /** Modes of operation (data read or write) */
    enum Mode { INVALID_MODE = 0, READ_MODE, WRITE_MODE};

    /** Flow types: cache, client, or server */
    enum Type {INVALID_TYPE = 0, CACHE_FLOW_TYPE, CLIENT_FLOW_TYPE, SERVER_FLOW_TYPE};

    /** Constructor for a cache operations */
    DataFlow(const spfsDataFlowStart& flowStart,
             std::size_t numBuffers,
             FSSize bufferSize,
             cSimpleModule* parentModule);

    virtual ~DataFlow() = 0;

    /** @return the buffer size used for network data flows */
    FSSize getBufferSize() const { return bufferSize_; };

    /** @return the size of the flow */
    FSSize getSize() const { return flowSize_; };

    /** @return the originating request */
    spfsDataFlowStart* getOriginatingMessage() const;

    /** @return the flow mode */
    Mode getMode() const { return mode_; };

    /** @return the unique id for this data flow */
    int getUniqueId() const { return uniqueId_; };

    /** Initialize the data flow (i.e. send the first message */
    void initialize();

    /** @return true when all flow data has been transferred */
    bool isComplete() const;

    /** @return true when the flow network interaction is completed */
    bool isNetworkComplete() const;

    /** @return true when the flow storage interaction is completed */
    bool isStorageComplete() const;

    /** @return the parent module for this flow */
    cSimpleModule* parentModule() const { return parentModule_; };

    /** Handle data flow messages */
    void handleServerMessage(cMessage* msg);

    /** @return the amount of data sent/received from the network */
    FSSize getNetworkProgress() const { return networkTransferTotal_; };

    /** @return the amount of data committed/retrieved from storage */
    FSSize getStorageProgress() const { return storageTransferTotal_; };

protected:
    /** Add dataTransferred to the transfer to the network total */
    void addNetworkProgress(FSSize dataTransferred);

    /** Add dataTransferred to the transfer to storage total */
    void addStorageProgress(FSSize dataTransferred);

    /** Start the flow of data */
    virtual void startTransfer() = 0;

    /** Perform the processing for a data flow message */
    virtual void processDataFlowMessage(cMessage* msg) = 0;

    /** Push data to the storage device */
    virtual void pushDataToStorage(FSSize pushSize) = 0;

    /** Pull data from the storage device */
    virtual void pullDataFromStorage(FSSize pullSize) = 0;

    /** Push data to the network */
    virtual void pushDataToNetwork(FSSize pushSize) = 0;

    /** Pull data from the network */
    virtual void pullDataFromNetwork(FSSize pullSize) = 0;

    /** Record the transfer from network delay */
    void collectTransferFromNetworkDelay(cMessage* response);

    /** Record the transfer from storage delay */
    void collectTransferFromStorageDelay(cMessage* response);

    /** Record the transfer to network delay */
    void collectTransferToNetworkDelay(cMessage* response);

    /** Record the transfer to storage delay */
    void collectTransferToStorageDelay(cMessage* response);

    /** Memory to disk layout of data being processed */
    DataTypeLayout layout_;

private:
    /** Initialize this object for a cache flow */
    void initCacheFlow();

    /** Initialize this object for a client flow */
    void initClientFlow();

    /** Initialize this object for a server flow */
    void initServerFlow();

    /**
     * @return the time between the origination of the request and
     *  this response.
     */
    simtime_t getRoundTripDelay(cMessage* response) const;

    /** The originating flow begin */
    const spfsDataFlowStart* originatingMessage_;

    /** The number of flow buffers to use */
    std::size_t numBuffers_;

    /** The size of a flow buffer */
    FSSize bufferSize_;

    /** Parent module */
    cSimpleModule* parentModule_;

    /** Indicate whether the flow is processing a read or write */
    Mode mode_;

    /** Indicate whether the type of flow to perform (cache, client, or server) */
    Type type_;

    /** Unique identifier */
    int uniqueId_;

    /** Size of the flow */
    FSSize flowSize_;

    /** Amount of data transferred to/from the network */
    FSSize networkTransferTotal_;

    /** Amount of data transferred to/from storage */
    FSSize storageTransferTotal_;

    // Data collection
    cOutVector transferFromNetworkDelay_;
    cOutVector transferFromStorageDelay_;
    cOutVector transferToNetworkDelay_;
    cOutVector transferToStorageDelay_;
};

#endif

/*
 * Local variables:
 *  indent-tabs-mode: nil
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 *
 * vim: ts=4 sts=4 sw=4 expandtab
 */
