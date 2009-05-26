#ifndef MIDDLEWARE_AGGREGATOR_H_
#define MIDDLEWARE_AGGREGATOR_H_
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
#include <cstddef>
#include <omnetpp.h>
#include "direct_message_interface.h"
class Filename;

/**
 * An abstract model of a middleware I/O aggregator.
 */
class MiddlewareAggregator : public cSimpleModule
{
public:
    /** Constructor */
    MiddlewareAggregator();

    /** Abstract destructor */
    virtual ~MiddlewareAggregator() = 0;

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

    /** Set the MPI world rank */
    void setRank(int rank) { rank_ = rank; };

    /** Return the world rank */
    int getRank() const { return rank_; };

    /** Add the delay associated with copying the memory in and out of the cache */
    void addCacheMemoryDelay(cMessage* origRequest, double delay) const;

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

private:
    /** Interface for handling messages from the application */
    virtual void handleApplicationMessage(cMessage* msg) = 0;

    /** Interface for handling messages from the file system */
    virtual void handleFileSystemMessage(cMessage* msg) = 0;

    /** The time to copy a byte of data to/from the cache */
    double byteCopyTime_;

    /** MPI World rank */
    int rank_;

    /** Gate ids */
    int appInGateId_;
    int appOutGateId_;
    int ioInGateId_;
    int ioOutGateId_;
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
