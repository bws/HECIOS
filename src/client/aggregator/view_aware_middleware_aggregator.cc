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
    void handleCollectiveIO(spfsMPIFileRequest* fileRequest);

    CollectiveMap* currentCollective_;

    CollectiveRequestMap* pendingCollectives_;

    int aggregatorSize_;
};

// OMNet Registration Method
Define_Module(ViewAwareMiddlewareAggregator);

void ViewAwareMiddlewareAggregator::initialize()
{
    MiddlewareAggregator::initialize();
    currentCollective_ = createCollectiveMap();
    pendingCollectives_ = createPendingRequestMap();
    setAggregatorSize(8);
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
            handleCollectiveIO(fileRequest);
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
    send(msg, appOutGateId());
}

void ViewAwareMiddlewareAggregator::handleCollectiveIO(spfsMPIFileRequest* fileRequest)
{
    CommMan& commMgr = CommMan::instance();
    Communicator comm = fileRequest->getCommunicator();
    currentCollective_->insert(fileRequest);
    if (currentCollective_->size() == aggregatorSize_)
    {
        CollectiveMap::const_iterator first = currentCollective_->begin();
        CollectiveMap::const_iterator last = currentCollective_->end();
        while (first != last)
        {
            send(*(first++), ioOutGateId());
        }
        currentCollective_->clear();
    }
    else
    {
        cerr << "Waiting bitches" << endl;
    }
    cerr << __FILE__ << ":" << __LINE__ << ":"
         << "Handling collective comm: " << fileRequest->getCommunicator()
         << " rank: " << fileRequest->getRank()
         << " of: " << commMgr.commSize(comm)
         << endl;
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
