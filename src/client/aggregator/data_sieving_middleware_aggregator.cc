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
#include "middleware_aggregator.h"
using namespace std;

/** Model of an aggregator that does Argonne data sieving semantics */
class DataSievingMiddlewareAggregator : public MiddlewareAggregator
{
public:
    /** Constructor */
    DataSievingMiddlewareAggregator();

private:
    /** Forward application messages to file system */
    virtual void handleApplicationMessage(cMessage* msg);

    /** Forward application messages to file system */
    virtual void handleFileSystemMessage(cMessage* msg);
};

// OMNet Registration Method
Define_Module(DataSievingMiddlewareAggregator);

DataSievingMiddlewareAggregator::DataSievingMiddlewareAggregator()
{
}

// Perform simple pass through on all messages
void DataSievingMiddlewareAggregator::handleApplicationMessage(cMessage* msg)
{
    send(msg, ioOutGateId());
}

void DataSievingMiddlewareAggregator::handleFileSystemMessage(cMessage* msg)
{
    send(msg, appOutGateId());
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
