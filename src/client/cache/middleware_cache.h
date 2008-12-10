#ifndef MIDDLEWARE_CACHE_H_
#define MIDDLEWARE_CACHE_H_
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
#include <cstddef>
#include <omnetpp.h>
class Filename;

/**
 * An abstract model of a middleware file system data cache.
 */
class MiddlewareCache : public cSimpleModule
{
public:
    /** Constructor */
    MiddlewareCache();

    /** Abstract destructor */
    ~MiddlewareCache() = 0;

    /** @return the appIn gate id */
    int appInGateId() const { return appInGateId_; };

    /** @return the appOut gate id */
    int appOutGateId() const { return appOutGateId_; };

    /** @return the fsIn gate id */
    int fsInGateId() const { return fsInGateId_; };

    /** @return the fsOut gate id */
    int fsOutGateId() const { return fsOutGateId_; };

protected:
    /** Implementation of initialize */
    virtual void initialize();

    /** Implementation of finish */
    virtual void finish();

    /** Implementation of handleMessage */
    virtual void handleMessage(cMessage* msg);

private:
    /** Interface for handling messages from the application */
    virtual void handleApplicationMessage(cMessage* msg) = 0;

    /** Interface for handling messages from the file system */
    virtual void handleFileSystemMessage(cMessage* msg) = 0;

    /** Gate ids */
    int appInGateId_;
    int appOutGateId_;
    int fsInGateId_;
    int fsOutGateId_;

    /** The number of cache hits */
    std::size_t numCacheHits_;

    /** The number of cache misses */
    std::size_t numCacheMisses_;

    /** The number of cache evictions */
    std::size_t numCacheEvicts_;
};


/** Model of a cache that does simple pass through semantics */
class NoMiddlewareCache : public MiddlewareCache
{
public:
    /** Constructor */
    NoMiddlewareCache();

    /** Increment the file open count */
    void processFileOpen(const Filename& openName) {};

    /**
     * Decrement the file open count and flush dirty file data to disk if
     * this is the last close for this file
     */
    void processFileClose(const Filename& closeName) {};

private:
    /** Forward application messages to file system */
    virtual void handleApplicationMessage(cMessage* msg);

    /** Forward application messages to file system */
    virtual void handleFileSystemMessage(cMessage* msg);
};

#endif /*MIDDLEWARE_CACHE_H_*/

/*
 * Local variables:
 *  indent-tabs-mode: nil
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 *
 * vim: ts=8 sts=4 sw=4 expandtab
 */
