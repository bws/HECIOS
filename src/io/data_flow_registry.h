#ifndef DATA_FLOW_REGISTRY_H
#define DATA_FLOW_REGISTRY_H
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
class DataFlow;

/**
 * Registry for all system data flows
 */
class DataFlowRegistry
{
public:
    /** Singleton accessor function */
    static DataFlowRegistry& instance();

    /**
     * Register the flow
     *
     * @return the flow's unique id
     */
    int registerDataFlow(DataFlow* flow);

    /** Remove the flow from the registry */
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
    
private:
    /** Default constructor */
    DataFlowRegistry();

    /** Destructor */
    ~DataFlowRegistry();
    
    /** Copy constructor hidden/disabled */
    DataFlowRegistry(const DataFlowRegistry& other);

    /** Assignment operator hidden/disabled */
    DataFlowRegistry& operator=(DataFlowRegistry& other);

    /** Singleton instance */
    static DataFlowRegistry* instance_;
    
    /** Map of flows by flow id */
    std::map<int, DataFlow*> dataFlowsById_;

    /** Map of flows by subscribed BMI tags */
    std::map<int, DataFlow*> dataFlowsByBMITag_;
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
