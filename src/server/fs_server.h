#ifndef FS_SERVER_H
#define FS_SERVER_H
//
// This file is part of Hecios
//
// Copyright (C) 2007,2008,2009 Brad Settlemyer
//
// This file is distributed WITHOUT ANY WARRANTY. See the file 'License.txt'
// for details on this and other legal matters.
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

    /** Set the change directory entry processing delay */
    static void setChangeDirEntProcessingDelay(simtime_t changeDirEntDelay);

    /** @return the server processing delay for changing a directory entry */
    static double changeDirEntProcessingDelay();

    /** Set the create data file processing delay */
    static void setCreateDFileProcessingDelay(simtime_t createDFileDelay);

    /** @return the server processing delay for creating a data object */
    static double createDFileProcessingDelay();

    /** Set the create directory processing delay */
    static void setCreateDirectoryProcessingDelay(simtime_t createDirDelay);

    /** @return the server processing delay for creating a directory */
    static double createDirectoryProcessingDelay();

    /** Set the create metadata processing delay */
    static void setCreateMetadataProcessingDelay(simtime_t createMetadataDelay);

    /** @return the server processing delay for creating a metadata object */
    static double createMetadataProcessingDelay();

    /** Set the create directory entry processing delay */
    static void setCreateDirEntProcessingDelay(simtime_t createDirEntDelay);

    /** @return the server processing delay for creating a directory entry */
    static double createDirEntProcessingDelay();

    /** Set the attribute retrieval processing delay */
    static void setGetAttrProcessingDelay(simtime_t getAttrDelay);

    /** @return the server processing delay for getting object attributes */
    static double getAttrProcessingDelay();

    /** Set the Path resolution processing delay */
    static void setLookupPathProcessingDelay(simtime_t lookupPathDelay);

    /** @return the server processing delay for resolving a path */
    static double lookupPathProcessingDelay();

    /** Set the read directory processing delay */
    static void setReadDirProcessingDelay(simtime_t readDirDelay);

    /** @return the server processing delay for reading directory entries */
    static double readDirProcessingDelay();

    /** Set the remove directory entry processing delay */
    static void setRemoveDirEntProcessingDelay(simtime_t removeDirEntDelay);

    /** @return the server processing delay for removing a directory entry */
    static double removeDirEntProcessingDelay();

    /** Set the remove metadata processing delay */
    static void setRemoveMetaProcessingDelay(simtime_t removeMetaDelay);

    /** @return the server processing delay for removing metadata */
    static double removeMetaProcessingDelay();

    /** Set the remove object processing delay */
    static void setRemoveObjectProcessingDelay(simtime_t removeObjectDelay);

    /** @return the server processing delay for removing an object */
    static double removeObjectProcessingDelay();

    /** Set the attributes set processing delay */
    static void setSetAttrProcessingDelay(simtime_t setAttrDelay);

    /** @return the server processing delay for setting object attributes */
    static double setAttrProcessingDelay();

    /** Set the server overhead delay */
    static void setServerOverheadDelay(simtime_t serverOverheadDelay);

    /** Set disk data collection on or off */
    static void setCollectDiskData(bool collectFlag);

    /** Constructor */
    FSServer();

    /** @return the server's unique name */
    std::string getServerName() const { return serverName_; };

    /** @return the server's unique number */
    std::size_t getServerNumber() const { return serverNumber_; };

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

    /** Record that a change dir ent request has arrived */
    void recordChangeDirEnt();

    /** Record that a collective create request has arrived */
    void recordCollectiveCreate();

    /** Record that a collective get attributes request has arrived */
    void recordCollectiveGetAttr();

    /** Record that a collective remove request has arrived */
    void recordCollectiveRemove();

    /** Record that a create dirent request has arrived */
    void recordCreateDirEnt();

    /** Record that a create object request has arrived */
    void recordCreateObject();

    /** Record that a get attributes request has arrived */
    void recordGetAttr();

    /** Record that a lookup path request has arrived */
    void recordLookup();

    /** Record that a read directory request has arrived */
    void recordReadDir();

    /** Record that a read pages request has arrived */
    void recordReadPages();

    /** Record that a read request has arrived */
    void recordRead();

    /** Record that a remove directory entry request has arrived */
    void recordRemoveDirEnt();

    /** Record that a remove object request has arrived */
    void recordRemoveObject();

    /** Record that a set attributes dirent request has arrived */
    void recordSetAttr();

    /** Record that a write request has arrived */
    void recordWrite();

    /** Record the disk delay for changing a directory entry */
    void recordChangeDirEntDiskDelay(cMessage* fileWriteResponse);

    /** Record the disk delay for creating directory entries */
    void recordCreateDirEntDiskDelay(cMessage* fileWriteResponse);

    /** Record the disk delay for creating objects */
    void recordCreateObjectDiskDelay(cMessage* fileOpenResponse);

    /** Record the disk delay for retrieving attributes */
    void recordGetAttrDiskDelay(cMessage* fileReadResponse);

    /** Record the disk delay for performing name lookup */
    void recordLookupDiskDelay(cMessage* fileReadResponse);

    /** Record the disk delay for reading PFS data */
    void recordReadDirDataDelay(cMessage* fileReadResponse);

    /** Record the disk delay for read directory entries */
    void recordReadDirDiskDelay(cMessage* fileReadResponse);

    /** Record the disk delay for removing a directory entry */
    void recordRemoveDirEntDiskDelay(cMessage* fileWriteResponse);

    /** Record the disk delay for removing an object */
    void recordRemoveObjectDiskDelay(cMessage* fileUnlinkResponse);

    /** Record the disk delay for setting attributes */
    void recordSetAttrDiskDelay(cMessage* fileWriteResponse);

    /** Record the disk delay for writing PFS data */
    void recordWriteDataDiskDelay(cMessage* fileWriteResponse);

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

    /** Change Directory Entry server processing delay */
    static double changeDirEntProcessingDelay_;

    /** Create DFile server processing delay*/
    static double createDFileProcessingDelay_;

    /** Create directory server processing delay*/
    static double createDirectoryProcessingDelay_;

    /** Create metadata server processing delay*/
    static double createMetadataProcessingDelay_;

    /** Create Directory Entry server processing delay */
    static double createDirEntProcessingDelay_;

    /** Get Attributes server processing delay */
    static double getAttrProcessingDelay_;

    /** Lookup Path server processing delay */
    static double lookupPathProcessingDelay_;

    /** Read Directory server processing delay */
    static double readDirProcessingDelay_;

    /** Remove Directory Entry server processing delay */
    static double removeDirEntProcessingDelay_;

    /** Remove Object server processing delay */
    static double removeObjectProcessingDelay_;

    /** Remove metadata server processing delay */
    static double removeMetaProcessingDelay_;

    /** Set Attributes server processing delay */
    static double setAttrProcessingDelay_;

    /** Server overhead delay */
    static double serverOverheadDelay_;

    /** Data collection flag */
    static bool collectDiskData_;

    /** Unique server number */
    std::size_t serverNumber_;

    /** Unique server name */
    std::string serverName_;

    /** The handle range for this server */
    HandleRange range_;

    /** Gate ids */
    int inGateId_;
    int outGateId_;

    /** Data collection scalars */
    double numChangeDirEnts_;
    double numCollectiveCreates_;
    double numCollectiveGetAttrs_;
    double numCollectiveRemoves_;
    double numCreateDirEnts_;
    double numCreateObjects_;
    double numGetAttrs_;
    double numLookups_;
    double numReadDirs_;
    double numReads_;
    double numReadPages_;
    double numRemoveDirEnts_;
    double numRemoveObjects_;
    double numSetAttrs_;
    double numWrites_;

    /** Data collection vectors */
    cOutVector changeDirEntDiskDelay_;
    cOutVector collectiveCreateDiskDelay_;
    cOutVector collectiveGetAttrDiskDelay_;
    cOutVector collectiveRemoveDiskDelay_;
    cOutVector createDirEntDiskDelay_;
    cOutVector createObjectDiskDelay_;
    cOutVector getAttrDiskDelay_;
    cOutVector lookupDiskDelay_;
    cOutVector readDataDiskDelay_;
    cOutVector readDirDiskDelay_;
    cOutVector removeDirEntDiskDelay_;
    cOutVector removeObjectDiskDelay_;
    cOutVector setAttrDiskDelay_;
    cOutVector writeDataDiskDelay_;
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
