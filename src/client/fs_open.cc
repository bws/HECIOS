#include <iostream>
#define FSM_DEBUG  // Enable FSM Debug output
#include <omnetpp.h>
#include "client_fs_state.h"
#include "fs_module.h"
#include "mpiio_proto_m.h"
#include "pfs_utils.h"
#include "pvfs_proto_m.h"
#include "fs_open.h"
using namespace std;

FSOpen::FSOpen(fsModule* module, spfsMPIFileOpenRequest* openReq)
    : fsModule_(module),
      openReq_(openReq)
{
    assert(0 != fsModule_);
    assert(0 != openReq_);
}

// Processing that occurs upon receipt of an MPI-IO Open request
void FSOpen::handleMessage(cMessage* msg)
{
    /** Restore the existing state for this Open Request */
    cFSM currentState = openReq_->getState();

    /** File system open state machine states */
    enum {
        INIT = 0,
        LOOKUP = FSM_Steady(1),
        CREATE_META = FSM_Steady(2),
        CREATE_DATA = FSM_Steady(3),
        WRITE_ATTR = FSM_Steady(4),
        WRITE_DIRENT = FSM_Steady(5),
        READ_ATTR = FSM_Steady(6),
        FINISH = FSM_Steady(7),
        ERRNF = FSM_Steady(8),
        ERREXCL = FSM_Steady(9),        
    };

    FSM_Switch(currentState)
    {
        case FSM_Exit(INIT):
        {
            assert(0 != dynamic_cast<spfsMPIFileOpenRequest*>(msg));
            bool isInDirCache, isInAttrCache;
            exitInit(static_cast<spfsMPIFileOpenRequest*>(msg),
                     isInDirCache, isInAttrCache);
            if (isInDirCache && isInAttrCache)
                FSM_Goto(currentState, FINISH);
            else if (isInDirCache)
                FSM_Goto(currentState, READ_ATTR);
            else
                FSM_Goto(currentState, LOOKUP);
            break;
        }
        case FSM_Enter(LOOKUP):
        {    
            assert(0 != dynamic_cast<spfsMPIFileOpenRequest*>(msg));
            enterLookup(static_cast<spfsMPIFileOpenRequest*>(msg));
            break;
        }
        case FSM_Exit(LOOKUP):
        {
            assert(0 != dynamic_cast<spfsMPIFileOpenRequest*>(msg));
            exitLookup(static_cast<spfsMPIFileOpenRequest*>(msg));
            break;
        }
        case FSM_Enter(CREATE_META):
        {    
            assert(0 != dynamic_cast<spfsCreateResponse*>(msg));
            enterCreateMeta();
            break;
        }
        case FSM_Exit(CREATE_META):
        {
            assert(0 != dynamic_cast<spfsCreateResponse*>(msg));
            exitCreateMeta(static_cast<spfsCreateResponse*>(msg));
            FSM_Goto(currentState, CREATE_DATA);
            break;
        }
        case FSM_Enter(CREATE_DATA):
            assert(0 != dynamic_cast<spfsCreateResponse*>(msg));
            enterCreateData();
            break;
        case FSM_Exit(CREATE_DATA):
            assert(0 != dynamic_cast<spfsCreateResponse*>(msg));
            exitCreateData(static_cast<spfsCreateResponse*>(msg));
            break;
        case FSM_Enter(WRITE_ATTR):
            assert(0 != dynamic_cast<spfsSetAttrResponse*>(msg));
            enterWriteAttr();
            break;
        case FSM_Exit(WRITE_ATTR):
            assert(0 != dynamic_cast<spfsSetAttrResponse*>(msg));
            exitWriteAttr(static_cast<spfsSetAttrResponse*>(msg));
            break;
        case FSM_Enter(WRITE_DIRENT):
        {
            assert(0 != dynamic_cast<spfsCreateDirEntResponse*>(msg));
            enterWriteDirEnt();
            break;
        }
        case FSM_Exit(WRITE_DIRENT):
        {
            assert(0 != dynamic_cast<spfsCreateDirEntResponse*>(msg));
            exitWriteDirEnt(static_cast<spfsCreateDirEntResponse*>(msg));
            FSM_Goto(currentState, FINISH);
            break;
        }
        case FSM_Enter(READ_ATTR):
        {    
            assert(0 != dynamic_cast<spfsCreateDirEntResponse*>(msg));
            enterReadAttr();
            break;
        }
        case FSM_Exit(READ_ATTR):
        {
            assert(0 != dynamic_cast<spfsGetAttrResponse*>(msg));
            exitReadAttr(static_cast<spfsGetAttrResponse*>(msg));
            FSM_Goto(currentState, FINISH);
            break;
        }
        case FSM_Enter(FINISH):
        {
            assert(0 != dynamic_cast<spfsMPIFileOpenResponse*>(msg));
            enterFinish();
            break;
        }
        default:
            cerr << "Error: Illegal open state entered: " << currentState
                 << endl;
    }

    openReq_->setState(currentState);
}

void FSOpen::exitInit(spfsMPIFileOpenRequest* openReq,
                      bool& outIsInDirCache,
                      bool& outIsInAttrCache)
{
    // Preconditions
    assert(0 != openReq->getFileName());
    assert(0 != openReq->getFiledes());
    
    // Initialize outbound variable
    outIsInDirCache = false;
    outIsInAttrCache = false;
    
    /* look for dir in cache */
    FSOpenFile* fd = static_cast<FSOpenFile*>(openReq->getFiledes());
    FSHandle* lookup = fsModule_->fsState().lookupDir(openReq->getFileName());
    if (0 == lookup)
    {
        /* dir entry not in cache go do lookup */
        fd->path = openReq->getFileName();
        PFSUtils::instance().parsePath(fd);
        fd->handles[0] = fsModule_->fsState().root();
        fd->curseg = 0;
    }
    else
    {
        // Found directory cache entry
        outIsInDirCache = true;
        
        /* dir entry in cache look for metadata in cache */
        FSMetaData* meta = fsModule_->fsState().lookupAttr(fd->handle);
        if (0 != meta)
        {
            // Found attribute cache entry
            outIsInAttrCache = true;
            
            /* metadata in cache, were all done */
            fd->metaData = *meta;
        }
    }
}

void FSOpen::enterLookup(spfsMPIFileOpenRequest* openReq)
{
    // Create the new request
    spfsLookupPathRequest* req = new spfsLookupPathRequest(
        0, SPFS_LOOKUP_PATH_REQUEST);
    req->setContextPointer(openReq);
    
    /* copy path to look up */
    FSOpenFile *filedes = (FSOpenFile *)openReq->getFiledes();
    req->setPath(
        filedes->path.substr(filedes->segstart[filedes->curseg],
                             filedes->seglen[filedes->curseg]).c_str());
    
    /* get handle for root dir */
    req->setHandle(filedes->handles[filedes->curseg]);
    fsModule_->send(req, fsModule_->fsNetOut);
}

void FSOpen::exitLookup(spfsMPIFileOpenRequest* openReq)
{
}

void FSOpen::enterCreateMeta()
{
    /* build a request message */
    spfsCreateRequest* req = new spfsCreateRequest(0, SPFS_CREATE_REQUEST);
    req->setContextPointer(openReq_);

    /* hash path to meta server number */
    int metaserver = fsModule_->fsState().hashPath(openReq_->getFileName());

    /* use first handle in range to address server */
    req->setHandleRng(fsModule_->fsState().servers(metaserver));
    req->setServerNo(-1);
    fsModule_->send(req, fsModule_->fsNetOut);
}

void FSOpen::exitCreateMeta(spfsCreateResponse* createResp)
{
    FSOpenFile* fd = static_cast<FSOpenFile*>(openReq_->getFiledes());
    fd->handle = createResp->getHandle();
    fd->metaData.metaHandle = fd->handle;
}

void FSOpen::enterCreateData()
{
    spfsCreateRequest *req;
    int numservers;
    int snum;
    /* build a request message */
    //FSOpenFile* fd = static_cast<FSOpenFile*>(openReq_->getFiledes());
    req = new spfsCreateRequest(0, SPFS_CREATE_REQUEST);
    req->setContextPointer(openReq_);
    numservers = fsModule_->fsState().defaultNumServers();
    /* send request to each server */
    for (snum = 0; snum < numservers; snum++)
    {
        int dataserver;
        spfsCreateRequest *newreq = (spfsCreateRequest *)req->dup();
        newreq->setServerNo(snum);
        /* randomly select a server from 0 to S-1 */
        dataserver = fsModule_->fsState().selectServer();
        /* get the bucket range for that server */
        newreq->setHandleRng(fsModule_->fsState().servers(dataserver));
        /* use first handle of range to address server */
        newreq->setHandle(fsModule_->fsState().servers(dataserver).first);
        fsModule_->send(newreq, fsModule_->fsNetOut);
    }
    /* free req */
    delete req;
    /* indicates how many responses we are waiting for */
    openReq_->setResponses(numservers);
}

void FSOpen::exitCreateData(spfsCreateResponse* createResp)
{
}

void FSOpen::enterWriteAttr()
{
    FSOpenFile* fd = static_cast<FSOpenFile*>(openReq_->getFiledes());
    spfsSetAttrRequest *req = new spfsSetAttrRequest(0, SPFS_SET_ATTR_REQUEST);
    req->setContextPointer(openReq_);
    req->setHandle(fd->handle);
    req->setMeta(fd->metaData);
    fsModule_->send(req, fsModule_->fsNetOut);
}

void FSOpen::exitWriteAttr(spfsSetAttrResponse* sattrResp)
{
}

void FSOpen::enterWriteDirEnt()
{
    spfsCreateDirEntRequest *req;
    req = new spfsCreateDirEntRequest(0, SPFS_CREATE_DIR_ENT_REQUEST);
    req->setContextPointer(openReq_);
    /* this addresses the server with the dir on it */
    FSOpenFile* fd = static_cast<FSOpenFile*>(openReq_->getFiledes());
    req->setHandle(fd->handles[fd->curseg]);
    /* this is the same handle, the handle of the parent dir */
    /* can we get rid of this field?  WBL */
    req->setParent_handle(fd->handles[fd->curseg]);
    /* this is the handle of the new file goes in dirent */
    req->setNew_handle(fd->metaData.metaHandle);
    /* this is the name of the file */
    req->setEntry(openReq_->getFileName());
    fsModule_->send(req, fsModule_->fsNetOut);   
}

void FSOpen::exitWriteDirEnt(spfsCreateDirEntResponse* dirEntResp)
{
    // FIXME do anything ?? */
    /* free create dirent response message */
    delete dirEntResp;

}

void FSOpen::enterReadAttr()
{
    FSOpenFile* fd = static_cast<FSOpenFile*>(openReq_->getFiledes());
    spfsGetAttrRequest *req = new spfsGetAttrRequest(0, SPFS_GET_ATTR_REQUEST);
    req->setContextPointer(openReq_);
    req->setHandle(fd->handle);
    fsModule_->send(req, fsModule_->fsNetOut);
}

void FSOpen::exitReadAttr(spfsGetAttrResponse* gattrResp)
{
    // FIXME install attributes into attribute cache

    /* free get attr resp message */
    delete gattrResp;
}

void FSOpen::enterFinish()
{
    spfsMPIFileOpenResponse *resp = new spfsMPIFileOpenResponse(
        0, SPFS_MPI_FILE_OPEN_RESPONSE);
    resp->setContextPointer(openReq_->contextPointer());
    resp->setFiledes(openReq_->getFiledes());
    fsModule_->send(resp, fsModule_->fsMpiOut);

    delete openReq_;
    openReq_ = 0;
}
            
/*
 * Local variables:
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 *
 * vim: ts=4 sts=4 sw=4 expandtab foldmethod=marker
 */
