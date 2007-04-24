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

void FSOpen::handleFSRequest(spfsMPIFileOpenRequest* openReq,
                             fsModule* module)
{
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

    currentState_ = openReq->getState();
    FSM_Switch(currentState_)
    {
        case FSM_Exit(INIT):
        {    
            bool isInDirCache, isInAttrCache;
            exitInit(openReq, module, isInDirCache, isInAttrCache);
            if (isInDirCache && isInAttrCache)
                FSM_Goto(currentState_, FINISH);
            else if (isInDirCache)
                FSM_Goto(currentState_, READ_ATTR);
            else
                FSM_Goto(currentState_, LOOKUP);
            break;
        }
        case FSM_Enter(LOOKUP):
        {    
            enterLookup(openReq, module);
            break;
        }
        case FSM_Exit(LOOKUP):
        {    
            exitLookup(openReq);
            break;
        }
        case FSM_Enter(CREATE_META):
        {    
            enterCreateMeta(openReq);
            break;
        }
        case FSM_Exit(CREATE_META):
            exitCreateMeta(openReq);
            break;
        case FSM_Enter(CREATE_DATA):
            enterCreateData(openReq);
            break;
        case FSM_Exit(CREATE_DATA):
            exitCreateData(openReq);
            break;
        case FSM_Enter(WRITE_ATTR):
            enterWriteAttr(openReq);
            break;
        case FSM_Exit(WRITE_ATTR):
            exitWriteAttr(openReq);
            break;
        case FSM_Enter(WRITE_DIRENT):
            enterWriteDirEnt(openReq);
            break;
        case FSM_Exit(WRITE_DIRENT):
            exitWriteDirEnt(openReq);
            break;
        case FSM_Enter(READ_ATTR):
            enterReadAttr(openReq);
            break;
        case FSM_Exit(READ_ATTR):
            exitReadAttr(openReq);
            break;
        case FSM_Enter(FINISH):
            enterFinish(openReq);
            break;
        default:
            cerr << "Error: Illegal open state entered: " << currentState_
                 << endl;
    }
}

void FSOpen::exitInit(spfsMPIFileOpenRequest* openReq,
                      fsModule* module,
                      bool& outIsInDirCache,
                      bool& outIsInAttrCache)
{
    // Preconditions
    assert(0 != openReq->getFileName());
    assert(0 != openReq->getFiledes());
    assert(0 != module);
    cerr << "File: " << openReq->getFileName() << endl;
    
    // Initialize outbound variable
    outIsInDirCache = false;
    outIsInAttrCache = false;
    
    /* look for dir in cache */
    FSOpenFile* fd = static_cast<FSOpenFile*>(openReq->getFiledes());
    FSHandle* lookup = module->fsState().lookupDir(openReq->getFileName());
    if (0 == lookup)
    {
        /* dir entry not in cache go do lookup */
        fd->path = openReq->getFileName();
        PFSUtils::instance().parsePath(fd);
        fd->handles[0] = module->fsState().root();
        fd->curseg = 0;
    }
    else
    {
        // Found directory cache entry
        outIsInDirCache = true;
        
        /* dir entry in cache look for metadata in cache */
        FSMetaData* meta = module->fsState().lookupAttr(fd->handle);
        if (0 != meta)
        {
            // Found attribute cache entry
            outIsInAttrCache = true;
            
            /* metadata in cache, were all done */
            fd->metaData = *meta;
        }
    }
}

void FSOpen::enterLookup(spfsMPIFileOpenRequest* openReq,
                         fsModule* module)
{
    cerr << "enterLookup" << endl;
    // Create the new request
    spfsLookupPathRequest* req = new spfsLookupPathRequest(
        0, SPFS_LOOKUP_PATH_REQUEST);
    req->setContextPointer(openReq);
    
    /* copy path to look up */
    FSOpenFile *filedes = (FSOpenFile *)openReq->getFiledes();
    req->setPath(filedes->path.substr(
                     filedes->segstart[filedes->curseg],
                     filedes->seglen[filedes->curseg]).c_str());
    
    /* get handle for root dir */
    req->setHandle(filedes->handles[filedes->curseg]);
    module->send(req, module->fsNetOut);
}

void FSOpen::exitLookup(spfsMPIFileOpenRequest* openReq)
{
}

void FSOpen::enterCreateMeta(spfsMPIFileOpenRequest* openReq)
{
}

void FSOpen::exitCreateMeta(spfsMPIFileOpenRequest* openReq)
{
}

void FSOpen::enterCreateData(spfsMPIFileOpenRequest* openReq)
{
}

void FSOpen::exitCreateData(spfsMPIFileOpenRequest* openReq)
{
}

void FSOpen::enterWriteAttr(spfsMPIFileOpenRequest* openReq)
{
}

void FSOpen::exitWriteAttr(spfsMPIFileOpenRequest* openReq)
{
}

void FSOpen::enterWriteDirEnt(spfsMPIFileOpenRequest* openReq)
{
}

void FSOpen::exitWriteDirEnt(spfsMPIFileOpenRequest* openReq)
{
}

void FSOpen::enterReadAttr(spfsMPIFileOpenRequest* openReq)
{
}

void FSOpen::exitReadAttr(spfsMPIFileOpenRequest* openReq)
{
}

void FSOpen::enterFinish(spfsMPIFileOpenRequest* openReq)
{
}
            
/*
 * Local variables:
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 *
 * vim: ts=4 sts=4 sw=4 expandtab foldmethod=marker
 */
