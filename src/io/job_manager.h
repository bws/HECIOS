#ifndef JOB_MANAGER_H
#define JOB_MANAGER_H
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
#include <map>
#include <omnetpp.h>
#include "basic_types.h"
class DataFlow;
class spfsDataFlowStart;

/**
 * Model of server job manager - chiefly responsible for managing data flows
 */
class JobManager : public cSimpleModule
{
public:
    /**
     * Create the correct data flow type based on the data flow start
     * message
     *
     * @return the new data flow
     */
    DataFlow* createDataFlow(spfsDataFlowStart* flowStart);

    /**
     * Register the flow with the job manager
     *
     * @return the flow's unique id
     */
    int registerDataFlow(DataFlow* flow);

    /** Remove the flow from the server */
    void deregisterDataFlow(int flowId);

    /** @return the flow for flowId or null if no such flow exists */
    DataFlow* lookupDataFlow(int flowId) const;

    /** Allow a data flow to subscribe to messages from other flows */
    void subscribeDataFlowToTag(DataFlow* flow, int flowTag);

    /** @return the data flow subscribed to the flow tag */
    DataFlow* getSubscribedDataFlow(int flowTag) const;

    /** Remove tag subscriptions for flow tag*/
    void removeSubscriptionTag(int flowTag);

    /** Remove tag subscriptions for flow */
    void unsubscribeDataFlow(DataFlow* flow);

protected:

    /** Implementation of initialize */
    virtual void initialize();

    /** Implementation of finish */
    virtual void finish();

    /** Implementation of handleMessage */
    virtual void handleMessage(cMessage* msg);

private:

    /** Handle messages arriving from self */
    void handleSelfMessage(cMessage* msg);

    /** Handle messages arriving from the network */
    void handleNetworkMessage(cMessage* msg);

    /** Handle messages arriving from the parallel file system */
    void handlePFSMessage(cMessage* msg);

    /** Handle messages arriving from the storage system */
    void handleStorageMessage(cMessage* msg);

    /** Gate ids */
    int directInGateId_;
    int netInGateId_;
    int netOutGateId_;
    int pfsInGateId_;
    int pfsOutGateId_;
    int storageInGateId_;
    int storageOutGateId_;

    /** The number of buffers per flow */
    std::size_t flowBuffers_;

    /** The size of flow buffers */
    FSSize flowBufferSize_;
};

#endif

/*
 * Local variables:
 *  indent-tabs-mode: nil
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 *
 * vim: ts=8 sts=4 sw=4 expandtab
 */
