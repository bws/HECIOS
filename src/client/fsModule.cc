// file fsModule.cc
#include "fs_module.h"
#include <iostream>
#include "pfs_types.h"
#include "pvfs_proto_m.h"
#include "mpiio_proto_m.h"
#include "fs_open.h"
using namespace std;

// local function decls
static void fsUnknownMessage(cMessage *req, cMessage *resp,
                     fsModule *client);
static void fsProcessMessage(cMessage *req, cMessage *resp,
                     fsModule *client);
static void fsProcess_mpiFileCloseRequest( spfsMPIFileCloseRequest *mpireq,
                     cMessage *resp, fsModule *client );
static void fsProcess_mpiFileDeleteRequest( spfsMPIFileDeleteRequest *mpireq,
                     cMessage *resp, fsModule *client );
static void fsProcess_mpiFileSetSizeRequest( spfsMPIFileSetSizeRequest *mpireq,
                     cMessage *resp, fsModule *client );
static void fsProcess_mpiFilePreallocateRequest(
                     spfsMPIFilePreallocateRequest *mpireq, cMessage *resp,
                     fsModule *client);
static void fsProcess_mpiFileGetSizeRequest( spfsMPIFileGetSizeRequest *mpireq,
                     cMessage *resp, fsModule *client );
static void fsProcess_mpiFileReadRequest( spfsMPIFileReadRequest *mpireq,
                     cMessage *resp, fsModule *client );
static void fsProcess_mpiFileWriteRequest( spfsMPIFileWriteRequest *mpireq,
                     cMessage *resp, fsModule *client );
/*
static void fsProcess_fsCreateResponse( fsCreateResponse *msg );
static void fsProcess_fsRemoveResponse( fsRemoveResponse *msg );
static void fsProcess_fsReadResponse( fsReadResponse *msg );
static void fsProcess_fsWriteResponse( fsWriteResponse *msg );
static void fsProcess_fsGetAttrResponse( fsGetAttrResponse *msg );
static void fsProcess_fsSetAttrResponse( fsSetAttrResponse *msg );
static void fsProcess_fsLookupPathResponse( fsLookupPathResponse *msg );
static void fsProcess_fsCreateDirEntResponse(
                     fsCreateDirEntResponse *msg );
static void fsProcess_fsRemoveDirEntResponse(
                     fsRemoveDirEntResponse *msg );
static void fsProcess_fsChangeDirEntResponse(
                     fsChangeDirEntResponse *msg );
static void fsProcess_fsTruncateResponse( fsTruncateResponse *msg );
static void fsProcess_fsMakeDirResponse( fsMakeDirResponse *msg );
static void fsProcess_fsReadDirResponse( fsReadDirResponse *msg );
static void fsProcess_fsWriteCompletionResponse(
                     fsWriteCompletionResponse *msg );
static void fsProcess_fsFlushResponse( fsFlushResponse *msg );
static void fsProcess_fsStatResponse( fsStatResponse *msg );
static void fsProcess_fsListAttrResponse( fsListAttrResponse *msg );
*/
static void fsProcessTimer( cMessage *msg );

Define_Module(fsModule);

void fsModule::initialize()
{
    fsMpiOut = findGate("appOut");
    fsMpiIn = findGate("appIn");
    fsNetOut = findGate("netOut");
    fsNetIn = findGate("netIn");
}

void fsModule::finish()
{
    // need to free anything here?
}

void fsModule::handleMessage(cMessage *msg)
{
    if (msg->isSelfMessage())
        fsProcessTimer(msg);
    else
        fsProcessMessage(msg, NULL, this);
}

void fsProcessMessage(cMessage *req, cMessage *resp, fsModule *client)
{ /* Call message specific hander {{{1 */
    switch(req->kind())
    {
        case SPFS_MPI_FILE_OPEN_REQUEST:
        {
            FSOpen open(client, static_cast<spfsMPIFileOpenRequest*>(req));
            if (resp)
                open.handleMessage(resp);
            else
                open.handleMessage(req);
            break;
        }
        case SPFS_MPI_FILE_CLOSE_REQUEST :
            fsProcess_mpiFileCloseRequest((spfsMPIFileCloseRequest *)req,
                                          resp, client);
            break;
        case SPFS_MPI_FILE_DELETE_REQUEST :
            fsProcess_mpiFileDeleteRequest((spfsMPIFileDeleteRequest *)req,
                                           resp, client);
            break;
        case SPFS_MPI_FILE_SET_SIZE_REQUEST :
            fsProcess_mpiFileSetSizeRequest((spfsMPIFileSetSizeRequest *)req,
                                            resp, client);
            break;
        case SPFS_MPI_FILE_PREALLOCATE_REQUEST :
            fsProcess_mpiFilePreallocateRequest((
                                                    spfsMPIFilePreallocateRequest *)req,
                                                resp, client);
            break;
        case SPFS_MPI_FILE_GET_SIZE_REQUEST :
            fsProcess_mpiFileGetSizeRequest((spfsMPIFileGetSizeRequest *)req,
                                            resp, client);
            break;
        case SPFS_MPI_FILE_READ_REQUEST :
            fsProcess_mpiFileReadRequest((spfsMPIFileReadRequest *)req,
                                         resp, client);
            break;
        case SPFS_MPI_FILE_WRITE_REQUEST :
            fsProcess_mpiFileWriteRequest((spfsMPIFileWriteRequest *)req,
                                          resp, client);
            break;
            // response messages
        case SPFS_CREATE_RESPONSE :
        case SPFS_REMOVE_RESPONSE :
        case SPFS_READ_RESPONSE :
        case SPFS_WRITE_RESPONSE :
        case SPFS_GET_ATTR_RESPONSE :
        case SPFS_SET_ATTR_RESPONSE :
        case SPFS_LOOKUP_PATH_RESPONSE :
        case SPFS_CREATE_DIR_ENT_RESPONSE :
        case SPFS_REMOVE_DIR_ENT_RESPONSE :
        case SPFS_CHANGE_DIR_ENT_RESPONSE :
        case SPFS_TRUNCATE_RESPONSE :
        case SPFS_MAKE_DIR_RESPONSE :
        case SPFS_READ_DIR_RESPONSE :
        case SPFS_WRITE_COMPLETION_RESPONSE :
        case SPFS_FLUSH_RESPONSE :
        case SPFS_STAT_RESPONSE :
        case SPFS_LIST_ATTR_RESPONSE :
            cerr << "Calling process message after unpacking" << endl;
            fsProcessMessage((cMessage *)req->contextPointer(), req, client);
            break;
        default :
            fsUnknownMessage(req, resp, client);
            break;
    }

} /* }}}1 */

void fsUnknownMessage(cMessage *req, cMessage *resp, fsModule *client)
{
    cerr << "FsModule: Unknown Message: " << req->kind()
         << " " << req->info() << endl;
}

// messages from user/cache

void fsProcess_mpiFileCloseRequest( spfsMPIFileCloseRequest *mpireq,
                                      cMessage *resp, fsModule *client )
{
    /* free open file descriptor */
}

void fsProcess_mpiFileDeleteRequest( spfsMPIFileDeleteRequest *mpireq,
                                      cMessage *resp, fsModule *client )
{
}

void fsProcess_mpiFileSetSizeRequest( spfsMPIFileSetSizeRequest *mpireq,
                                      cMessage *resp, fsModule *client )
{
}

void fsProcess_mpiFilePreallocateRequest( spfsMPIFilePreallocateRequest *mpireq,
                                      cMessage *resp, fsModule *client )
{
}

void fsProcess_mpiFileGetSizeRequest( spfsMPIFileGetSizeRequest *mpireq,
                                      cMessage *resp, fsModule *client )
{
}

void fsProcess_mpiFileReadRequest( spfsMPIFileReadRequest *mpireq,
                                   cMessage *resp, fsModule *client )
{
/* process MPI file read request {{{1 */
    FSOpenFile *filedes;
    int count;
    MPIDataType dtype;
    unsigned int snum;
    cFSM state;
    enum{
        INIT = 0,
        READ = FSM_Steady(1),
    };
    state = mpireq->getState();
    FSM_Switch (state)
    {
        case FSM_Exit(INIT) :
	    filedes = (FSOpenFile *)mpireq->getFiledes();
	    count = mpireq->getCount();
	    dtype = mpireq->getDtype();
            FSM_Goto(state, READ);
            break;
        case FSM_Enter(READ) :
        {
            /* READ STATE {{{2 */
            spfsReadRequest *req;
            /* build a request  */
            req = new spfsReadRequest(0, SPFS_READ_REQUEST);
            req->setContextPointer(mpireq);
            req->setServerCnt(filedes->metaData->dataHandles.size());
            req->setOffset(filedes->filePtr);
            req->setCount(count);
            req->setDtype(dtype);
            /* send request to each server */
	    for (snum = 0; snum < filedes->metaData->dataHandles.size(); snum++)
	    {
                if (client->fsState().serverNotUsed(snum, filedes->metaData->dist,
                                               count, dtype))
                    /* don't send if no data on server */
                    continue;
                else
                {
                    spfsReadRequest *newreq = (spfsReadRequest *)req->dup();
                    newreq->setHandle(filedes->metaData->dataHandles[snum]);
                    req->setServerNo(snum);
                    client->send(newreq, client->fsNetOut);
                }
	    }
            /* free req */
            delete req;
            /* indicates how many responses we are waiting for */
            mpireq->setResponses(filedes->metaData->dataHandles.size());
            break;
        }
        case FSM_Exit(READ) :
        {
            int respcnt;
            /* handle read response */
            spfsReadResponse *fsresp = (spfsReadResponse *)resp;
            respcnt = mpireq->getResponses() - 1;
            if (respcnt <= 0)
            {
                //void *mpi_context;
                /* done with this request */
                spfsMPIFileReadResponse *mpiresp;
                mpiresp = new spfsMPIFileReadResponse(0, SPFS_MPI_FILE_READ_RESPONSE);
                mpiresp->setContextPointer(mpireq->contextPointer());
                mpiresp->setIsSuccessful(true);
                mpiresp->setBytes_read(fsresp->getBytes_read());
                client->send(mpiresp, client->fsMpiOut);
            }
            else
            {
                /* more responses comming */
                mpireq->setResponses(respcnt);
            }
            delete resp;
            break;
        } /* }}}2 */
        default :
            break;
    }
    mpireq->setState(state);
} /* }}}1 */

void fsProcess_mpiFileWriteRequest( spfsMPIFileWriteRequest *mpireq,
                                   cMessage *resp, fsModule *client )
{
    /* process MPI file read request {{{1 */
    FSOpenFile *filedes;
    int count;
    MPIDataType dtype;
    unsigned int snum;
    cFSM state;
    enum{
        INIT = 0,
        WRITE = FSM_Steady(1),
    };
    state = mpireq->getState();
    FSM_Switch (state)
    {
        case FSM_Exit(INIT) :
	    filedes = (FSOpenFile *)mpireq->getFiledes();
	    count = mpireq->getCount();
	    dtype = mpireq->getDtype();
            FSM_Goto(state, WRITE);
            break;
        case FSM_Enter(WRITE) :
        { /* WRITE STATE {{{2 */
	    spfsWriteRequest *req;
	    /* build a request  */
	    req = new spfsWriteRequest(0, SPFS_WRITE_REQUEST);
	    req->setContextPointer(mpireq);
	    req->setServer_cnt(filedes->metaData->dataHandles.size());
	    req->setOffset(filedes->filePtr);
	    req->setCount(count);
	    req->setDtype(dtype);
	    /* send request to each server */
	    for (snum = 0; snum < filedes->metaData->dataHandles.size(); snum++)
	    {
                if (client->fsState().serverNotUsed(snum, filedes->metaData->dist,
                                               count, dtype))
                    /* don't send if no data on server */
                    continue;
                else
                {
                    spfsWriteRequest *newreq = (spfsWriteRequest *)req->dup();
                    newreq->setHandle(filedes->metaData->dataHandles[snum]);
                    req->setServer_nr(snum);
                    client->send(newreq, client->fsNetOut);
                }
	    }
            /* free req */
            delete req;
            /* indicates how many responses we are waiting for */
            mpireq->setResponses(filedes->metaData->dataHandles.size());
            break;
        }
        case FSM_Exit(WRITE) :
        {
            int respcnt;
            /* handle read response */
            spfsWriteResponse *fsresp = (spfsWriteResponse *)resp;
            respcnt = mpireq->getResponses() - 1;
            if (respcnt <= 0)
            {
                //void *mpi_context;
                /* done with this request */
                spfsMPIFileWriteResponse *mpiresp;
                mpiresp = new spfsMPIFileWriteResponse(0, SPFS_MPI_FILE_WRITE_RESPONSE);
                mpiresp->setContextPointer(mpireq->contextPointer());
                mpiresp->setIsSuccessful(true);
                mpiresp->setBytes_written(fsresp->getBytes_written());
                client->send(mpiresp, client->fsMpiOut);
            }
            else
            {
                /* more responses comming */
                mpireq->setResponses(respcnt);
            }
            delete resp;
            break;
        } /* }}}2 */
        default :
            break;
    }
    mpireq->setState(state);
} /* }}}1 */

// messages from server/INET
/*
void fsProcess_fsCreateResponse( fsCreateResponse *msg )
{
}

void fsProcess_fsRemoveResponse( fsRemoveResponse *msg )
{
}

void fsProcess_fsReadResponse( fsReadResponse *msg )
{
}

void fsProcess_fsWriteResponse( fsWriteResponse *msg )
{
}

void fsProcess_fsGetAttrResponse( fsGetAttrResponse *msg )
{
}

void fsProcess_fsSetAttrResponse( fsSetAttrResponse *msg )
{
}

void fsProcess_fsLookupPathResponse( fsLookupPathResponse *msg )
{
}

void fsProcess_fsCreateDirEntResponse( fsCreateDirEntResponse *msg )
{
}

void fsProcess_fsRemoveDirEntResponse( fsRemoveDirEntResponse *msg )
{
}

void fsProcess_fsChangeDirEntResponse( fsChangeDirEntResponse *msg )
{
}

void fsProcess_fsTruncateResponse( fsTruncateResponse *msg )
{
}

void fsProcess_fsMakeDirResponse( fsMakeDirResponse *msg )
{
}

void fsProcess_fsReadDirResponse( fsReadDirResponse *msg )
{
}

void fsProcess_fsWriteCompletionResponse( fsWriteCompletionResponse *msg )
{
}

void fsProcess_fsFlushResponse( fsFlushResponse *msg )
{
}

void fsProcess_fsStatResponse( fsStatResponse *msg )
{
}

void fsProcess_fsListAttrResponse( fsListAttrResponse *msg )
{
}
*/
// timers from self
void fsProcessTimer( cMessage *msg )
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
