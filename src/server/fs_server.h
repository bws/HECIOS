#ifndef FS_SERVER_H
#define FS_SERVER_H
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

#include <string>
#include <omnetpp.h>
#include "pfs_types.h"
class spfsRequest;

/**
 * Model of a file system client library.
 */
class FSServer : public cSimpleModule
{
public:
    /** Constructor */
    FSServer() : cSimpleModule() {};

    /** @return the server's unique name */
    std::string getName() const { return serverName_; };

    /** @return the server's unique number */
    std::size_t getNumber() const { return serverNumber_; };

    /** @return the range of handles assigned to this server */
    HandleRange getHandleRange() const { return range_; };

    /** Set the server's unique number */
    void setNumber(size_t number);

    /** Set the server's unique handle range */
    void setHandleRange(const HandleRange& range) {range_ = range;};
    
protected:
    
    /** Implementation of initialize */
    virtual void initialize();

    /** Implementation of finish */
    virtual void finish() {};

    /** Implementation of handleMessage */
    virtual void handleMessage(cMessage* msg);

    /** Process incoming message according to the parent request type */
    void processMessage(spfsRequest* request, cMessage* msg);
    
private:

    /** Unique server number */
    std::size_t serverNumber_;

    /** Unique server name */
    std::string serverName_;

    /** The handle range for this server */
    HandleRange range_;

    /** Gate ids */
    int netInGateId_;
    int netOutGateId_;
    int storageInGateId_;
    int storageOutGateId_;
};

#endif

/*
 * Local variables:
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 *
 * vim: ts=8 sts=4 sw=4 expandtab
 */
