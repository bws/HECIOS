//
// This file is part of Hecios
//
// Copyright (C) 2007,2008,2009 Brad Settlemyer
//
// This file is distributed WITHOUT ANY WARRANTY. See the file 'License.txt'
// for details on this and other legal matters.
//
#include "data_flow_registry.h"
#include <cassert>
#include "data_flow.h"
using namespace std;

DataFlowRegistry* DataFlowRegistry::instance_ = 0;

DataFlowRegistry& DataFlowRegistry::instance()
{
    if (0 == instance_)
    {
        instance_ = new DataFlowRegistry();
    }
    return *instance_;
}

DataFlowRegistry::DataFlowRegistry()
{
}

int DataFlowRegistry::registerDataFlow(DataFlow* flow)
{
    assert(0 != flow);
    int uid = flow->getUniqueId();
    dataFlowsById_[uid] = flow;
    return uid;
}

void DataFlowRegistry::deregisterDataFlow(int flowId)
{
    assert(0 != lookupDataFlow(flowId));
    dataFlowsById_.erase(flowId);
}

DataFlow* DataFlowRegistry::lookupDataFlow(int flowId) const
{
    DataFlow* flow = 0;
    map<int, DataFlow*>::const_iterator pos = dataFlowsById_.find(flowId);
    if (dataFlowsById_.end() != pos)
    {
        flow = pos->second;
    }
    return flow;
}

void DataFlowRegistry::subscribeDataFlowToTag(DataFlow* flow, int flowTag)
{
    //cerr << name() << ": Subscribing flow for tag: " << flowTag << endl;
    assert(0 != flow);
    dataFlowsByBMITag_[flowTag] = flow;
}

DataFlow* DataFlowRegistry::getSubscribedDataFlow(int flowTag) const
{
    //cerr << name() << ": Looking up tag: " << flowTag << endl;
    DataFlow* flow = 0;
    map<int, DataFlow*>::const_iterator pos = dataFlowsByBMITag_.find(flowTag);
    if (dataFlowsByBMITag_.end() != pos)
    {
        flow = pos->second;
    }
    return flow;
}

void DataFlowRegistry::removeSubscriptionTag(int flowTag)
{
    assert(0 != getSubscribedDataFlow(flowTag));
    dataFlowsByBMITag_.erase(flowTag);
}

void DataFlowRegistry::unsubscribeDataFlow(DataFlow* flow)
{
    map<int, DataFlow*>::iterator iter;
    for (iter = dataFlowsByBMITag_.begin(); iter != dataFlowsByBMITag_.end(); ++iter)
    {
        if (iter->second == flow)
        {
            dataFlowsByBMITag_.erase(iter);
        }
    }
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
