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
#include <cstddef>
#include <map>
#include <string>
#include <omnetpp.h>
#include "pfs_types.h"
class spfsRequest;
class DataFlow;

/**
 * Model of a parallel file system server process.
 */
class FSServer : public cSimpleModule
{
public:
    /**
     * Includes the distribution, dfile count, metadata hint -> flags
     * NOTE: Also need to add the data file arrary
    */
    static const int METADATA_ATTRIBUTES_BYTE_SIZE = 4 + 4 + 4;

    /**
     * Includes the distribution name size, skip4, "simple_stripe",
     * distribution parameter size, skip4, "strip_size", dfile count
     * 8 + 4 + 4 + 14 + 4 + 4 + 11 + 4 = 49
     */
    static const int DIRECTORY_ATTRIBUTES_BYTE_SIZE = 49;

    /** TODO */
    static const int DATAFILE_ATTRIBUTES_BYTE_SIZE = 16;
    
    /** @return the default size of metadata/attributes */
    static std::size_t getDefaultAttrSize();

    /** @return the size of a directory entry */
    static std::size_t getDirectoryEntrySize();
    
    /** Set the default size of metadata/attributes */
    static void setDefaultAttrSize(std::size_t attrSize);

    /** Set the create object processing delay */
    static void setCreateObjectProcessingDelay(simtime_t createObjectDelay);

    /** @return the server processing delay for creating an object */
    static simtime_t createObjectProcessingDelay();
    
    /** Set the create directory entry processing delay */
    static void setCreateDirEntProcessingDelay(simtime_t createDirEntDelay);

    /** @return the server processing delay for creating a directory entry */
    static simtime_t createDirEntProcessingDelay();
    
    /** Set the attribute retrieval processing delay */
    static void setGetAttrProcessingDelay(simtime_t getAttrDelay);

    /** @return the server processing delay for getting object attributes */
    static simtime_t getAttrProcessingDelay();
    
    /** Set the Path resolution processing delay */
    static void setLookupPathProcessingDelay(simtime_t lookupPathDelay);

    /** @return the server processing delay for resolving a path */
    static simtime_t lookupPathProcessingDelay();
    
    /** Set the attributes set processing delay */
    static void setSetAttrProcessingDelay(simtime_t setAttrDelay);

    /** @return the server processing delay for setting object attributes */
    static simtime_t setAttrProcessingDelay();
    
    /** Constructor */
    FSServer();

    /** @return the server's unique name */
    std::string getName() const { return serverName_; };

    /** @return the server's unique number */
    std::size_t getNumber() const { return serverNumber_; };

    /** @return the range of handles assigned to this server */
    HandleRange getHandleRange() const { return range_; };

    /** @return true if handle is on this server */
    bool handleIsLocal(const FSHandle& handle) const;
    
    /** Set the server's unique number */
    void setNumber(std::size_t number);

    /** Set the server's unique handle range */
    void setHandleRange(const HandleRange& range) {range_ = range;};

    /** Send the message out of the PFS server */
    void send(cMessage* outMsg);

    /** Send the message out of the PFS server after delay */
    void sendDelayed(cMessage* outMsg, simtime_t delay);

    /** Record the disk delay for creating directory entries */
    void recordCreateDirEntDiskDelay(cMessage* fileWriteResponse);

    /** Record the disk delay for creating objects */
    void recordCreateObjectDiskDelay(cMessage* fileOpenResponse);

    /** Record the disk delay for retrieving attributes */
    void recordGetAttrDiskDelay(cMessage* fileReadResponse);

    /** Record the disk delay for performing name lookup */
    void recordLookupDiskDelay(cMessage* fileReadResponse);

    /** Record the disk delay for setting attributes */
    void recordSetAttrDiskDelay(cMessage* fileWriteResponse);
    
protected:
    
    /** Implementation of initialize */
    virtual void initialize();

    /** Implementation of finish */
    virtual void finish();

    /** Implementation of handleMessage */
    virtual void handleMessage(cMessage* msg);

    /** Process incoming message according to the parent request type */
    void processRequest(spfsRequest* request, cMessage* msg);
    
private:
    /**
     * @return the difference between the current time and originating req
     *    creation time
     */
    simtime_t getRoundTripDelay(cMessage* response) const;

    /** Default attribute size */
    static std::size_t defaultAttrSize_;

    /** Create object server processing delay*/
    static simtime_t createObjectProcessingDelay_;

    /** Create Directory Entry server processing delay */
    static simtime_t createDirEntProcessingDelay_;

    /** Get Attributes server processing delay */
    static simtime_t getAttrProcessingDelay_;

    /** Lookup Path server processing delay */
    static simtime_t lookupPathProcessingDelay_;

    /** Set Attributes server processing delay */
    static simtime_t setAttrProcessingDelay_;
    
    /** Unique server number */
    std::size_t serverNumber_;

    /** Unique server name */
    std::string serverName_;

    /** The handle range for this server */
    HandleRange range_;

    /** Gate ids */
    int inGateId_;
    int outGateId_;

    /** Data collection vectors */
    cOutVector createDirEntDiskDelay_;
    cOutVector createObjectDiskDelay_;
    cOutVector getAttrDiskDelay_;
    cOutVector lookupDiskDelay_;
    cOutVector setAttrDiskDelay_;
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
