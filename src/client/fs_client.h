#ifndef FS_CLIENT_H
#define FS_CLIENT_H
//
// This file is part of Hecios
//
// Copyright (C) 2007,2008,2009 Brad Settlemyer
//
// This file is distributed WITHOUT ANY WARRANTY. See the file 'License.txt'
// for details on this and other legal matters.
//
#include <omnetpp.h>
#include <vector>
#include "client_fs_state.h"
#include "pfs_types.h"
class FileDistribution;
class FileView;
class spfsCollectiveCreateRequest;
class spfsCollectiveGetAttrRequest;
class spfsCollectiveRemoveRequest;
class spfsCreateRequest;
class spfsCreateDirEntRequest;
class spfsGetAttrRequest;
class spfsLookupPathRequest;
class spfsReadDirRequest;
class spfsReadRequest;
class spfsRemoveDirEntRequest;
class spfsRemoveRequest;
class spfsSetAttrRequest;
class spfsWriteRequest;

class FSClient : public cSimpleModule
{
public:
    /** Attributes contains the following fields:
     *  UID(4), GID(4), Perms(4), ATime(8), MTime(8), CTime(8), (4), (4),
     *  (16)
     */
    static const unsigned int OBJECT_ATTRIBUTES_SIZE = 60;

    /** Credentials contains the following fields:
     *  UID(4), GID(4)
     */
    static const unsigned int CREDENTIALS_SIZE = 8;

    /** @return a new Collective File Create request */
    static spfsCollectiveCreateRequest* createCollectiveCreateRequest(
        const FSHandle& parentHandle,
        const FSHandle& metaHandle,
        std::vector<FSHandle> dataHandles);

    /** @return a new Collective GetAttr request */
    static spfsCollectiveGetAttrRequest* createCollectiveGetAttrRequest(
        const FSHandle& metaHandle,
        std::vector<FSHandle> dataHandles);

    /** @return a new Collective Remove request */
    static spfsCollectiveRemoveRequest* createCollectiveRemoveRequest(
        const FSHandle& parentHandle,
        const FSHandle& metaHandle,
        std::vector<FSHandle> dataHandles);

    /** @return a new Create request */
    static spfsCreateRequest* createCreateRequest(const FSHandle& handle,
                                                  FSObjectType objectType);

    /** @return a new Create DirEnt request */
    static spfsCreateDirEntRequest* createCreateDirEntRequest(
        const FSHandle& handle, const Filename& entry);

    /** @return a new GetAttr request */
    static spfsGetAttrRequest* createGetAttrRequest(const FSHandle& handle,
                                                    FSObjectType objectType);

    /** @return a new LookupPath request */
    static spfsLookupPathRequest* createLookupPathRequest(
        const Filename& lookupname,
        const FSHandle& handle,
        int numResolvedSegments);

    /** @return a new Read Dir request */
    static spfsReadDirRequest* createReadDirRequest(const FSHandle& handle,
                                                    std::size_t dirEntCount);

    /** @return a new Read Request */
    static spfsReadRequest* createReadRequest(const FSHandle& handle,
                                              const FileView& view,
                                              FSOffset offset,
                                              FSSize dataSize,
                                              const FileDistribution& dist);

    /** @return a new Remove DirEnt request */
    static spfsRemoveDirEntRequest* createRemoveDirEntRequest(
        const FSHandle& handle, const Filename& entry);

    /** @return a new Remove request */
    static spfsRemoveRequest* createRemoveRequest(const FSHandle& handle,
                                                  FSObjectType objectType);

    /** @return a new SetAttr request */
    static spfsSetAttrRequest* createSetAttrRequest(const FSHandle& handle,
                                                    FSObjectType objectType);

    /** @return a new Write Request */
    static spfsWriteRequest* createWriteRequest(const FSHandle& metaHandle,
                                                const FSHandle& dataHandle,
                                                const FileView& view,
                                                FSOffset offset,
                                                FSSize dataSize,
                                                const FileDistribution& dist);

    /** Constructor */
    FSClient();

    /** @return a reference to the client filesystem state */
    ClientFSState& fsState() { return clientState_; };

    /** @return the application outbound gate id */
    int getAppOutGate() const { return appOutGateId_; };

    /** @return the network outbound gate id */
    int getNetOutGate() const { return netOutGateId_; };

protected:
    /** Initialize the module */
    virtual void initialize();

    /** Finalize the module */
    virtual void finish();

    /** Handle incoming messages */
    virtual void handleMessage(cMessage *msg);

private:
    /** Schedule the incoming message after its associated processing delay */
    void scheduleRequest(cMessage* request);

    /**
     * Process the incoming message based on the originating request type
     *
     * @param the originating request type
     * @param the message to process
     */
    void processMessage(cMessage* request, cMessage* msg);

    /** Collect statistics on server responses */
    void collectServerResponseData(cMessage* serverResponse);

    /** Gate ids */
    int appInGateId_;
    int appOutGateId_;
    int netInGateId_;
    int netOutGateId_;

    /** Enable collective file creation optimization */
    bool useCollectiveCreate_;

    /** Enable collective file get attributes optimization */
    bool useCollectiveGetAttr_;

    /** Enable collective file remove optimization */
    bool useCollectiveRemove_;

    /** Delay associated with client processing */
    double clientOverheadDelay_;

    /** Client processing delay for directory creation */
    double directoryCreateProcessingDelay_;

    /** Client processing delay for directory read */
    double directoryReadProcessingDelay_;

    /** Client processing delay for directory removal */
    double directoryRemoveProcessingDelay_;

    /** Client processing delay for file close */
    double fileCloseProcessingDelay_;

    /** Client processing delay for file delete */
    double fileDeleteProcessingDelay_;

    /** Client processing delay for file open */
    double fileOpenProcessingDelay_;

    /** Client processing delay for file read */
    double fileReadProcessingDelay_;

    /** Client processing delay for file stat */
    double fileStatProcessingDelay_;

    /** Client processing delay for file utime */
    double fileUpdateTimeProcessingDelay_;

    /** Client processing delay for file write */
    double fileWriteProcessingDelay_;

    /** Client file system state */
    ClientFSState clientState_;

    /** Fixed data collection */
    double numACacheHits_;
    double numACacheMisses_;
    double numDCacheHits_;
    double numDCacheMisses_;
    double numDirCreates_;
    double numDirReads_;
    double numDirRemoves_;
    double numFileCloses_;
    double numFileDeletes_;
    double numFileOpens_;
    double numFileReads_;
    double numFileStats_;
    double numFileUtimes_;
    double numFileWrites_;
    double numCacheReadExclusives_;
    double numCacheReadShareds_;

    /** Temporal data collection */
    cOutVector collectiveCreateDelay_;
    cOutVector collectiveGetAttrDelay_;
    cOutVector collectiveRemoveDelay_;
    cOutVector createDirEntDelay_;
    cOutVector createObjectDelay_;
    cOutVector flowDelay_;
    cOutVector getAttrDelay_;
    cOutVector lookupPathDelay_;
    cOutVector readDirDelay_;
    cOutVector readDelay_;
    cOutVector removeDelay_;
    cOutVector removeDirEntDelay_;
    cOutVector setAttrDelay_;
    cOutVector writeCompleteDelay_;
    cOutVector writeDelay_;
};

#endif

/*
 * Local variables:
 *  indent-tabs-mode: nil
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 *
 * vim: ts=4 sts=4 sw=4 expandtab
 */
