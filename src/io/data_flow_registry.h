#ifndef DATA_FLOW_REGISTRY_H
#define DATA_FLOW_REGISTRY_H
//
// This file is part of Hecios
//
// Copyright (C) 2007,2008,2009 Brad Settlemyer
//
// This file is distributed WITHOUT ANY WARRANTY. See the file 'License.txt'
// for details on this and other legal matters.
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
