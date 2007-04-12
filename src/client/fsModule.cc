// file fsModule.cc
#include <omnetpp.h>
#include <pfs_types.h>
#include <pvfs_proto_m.h>
#include <mpiio_proto_m.h>
#include <client_fs_state.h>
class fsModule;

// local function decls
static void fsUnknownMessage(cMessage *req, cMessage *resp,
                     fsModule *client);
static void fsProcessMessage(cMessage *req, cMessage *resp,
                     fsModule *client);
static void fsProcess_mpiFileOpenRequest( spfsMPIFileOpenRequest *mpireq,
                     cMessage *resp, fsModule *client );
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
static void fsParsePath(struct FSOpenFile *fdes);

// omnet++ module decl
class fsModule : public cSimpleModule
{
	protected:
		virtual void initialize();
		virtual void finish();
		virtual void handleMessage(cMessage *msg);

	public:
		int fsMpiOut;
		int fsMpiIn;
		int fsNetOut;
		int fsNetIn;
};

Define_Module(fsModule);

void fsModule::initialize()
{
	fsMpiOut = findGate("fsMpiOut");
	fsMpiIn = findGate("fsMpiIn");
	fsNetOut = findGate("fsNetOut");
	fsNetIn = findGate("fsNetIn");
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
		case SPFS_MPI_FILE_OPEN_REQUEST :
			fsProcess_mpiFileOpenRequest((spfsMPIFileOpenRequest *)req,
                                      resp, client);
			break;
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
			fsProcessMessage((cMessage *)req->contextPointer(), req, client);
			break;
		default :
			fsUnknownMessage(req, resp, client);
			break;
		}
} /* }}}1 */

void fsUnknownMessage(cMessage *req, cMessage *resp, fsModule *client)
{
}

// messages from user/cache

void fsProcess_mpiFileOpenRequest( spfsMPIFileOpenRequest *mpireq,
                                   cMessage *resp, fsModule *client )
{ /* Handle MPI open request {{{1 */
    /* assume for the moment this is a response */
	FSOpenFile *filedes = (FSOpenFile *)mpireq->getFiledes();
    cFSM state;
	enum {
		INIT = 0,
		LOOKUP = FSM_Steady (1),
        CREATE_META = FSM_Steady(2),
        CREATE_DATA = FSM_Steady(3),
		WRITE_ATTR = FSM_Steady (4),
		WRITE_DIRENT = FSM_Steady (5),
		READ_ATTR = FSM_Steady (6),
		FINISH = FSM_Steady (7),
		ERRNF = FSM_Steady (8),
		ERREXCL = FSM_Steady (9),
	};
    state = mpireq->getState();
	FSM_Switch(state)
	{
	case FSM_Exit(INIT):
	{ /* INIT STATE {{{2 */
		/* create open file descriptor */
		filedes = new FSOpenFile;
		mpireq->setFiledes(filedes);
		filedes->fs = (ClientFSState*)mpireq->getFs();
		/* look for dir in cache */
		filedes->handle = filedes->fs->lookupDir(mpireq->getFileName());
		if (filedes->handle == 0) // how to do this?
		{
			/* dir entry not in cache go do lookup */
            filedes->path = mpireq->getFileName();
            fsParsePath(filedes);
            filedes->handles[0] = filedes->fs->fsRoot();
            filedes->curseg = 0;
			FSM_Goto(state, LOOKUP);
		}
		else
		{
			/* dir entry in cache look for metadata in cache */
			filedes->metaData = filedes->fs->lookupAttr(filedes->handle);
			if (filedes->metaData.metaHandle == 0)
			{
			   /* metadata not in cache go read attributes */
			   FSM_Goto(state, READ_ATTR);
			}
			else
			{
			   /* metadata in cache, were all done */
			   FSM_Goto(state, FINISH);
			}
		}
		break;
	} /* }}}2 */
	case FSM_Enter(LOOKUP):
	{ /* LOOKUP STATE {{{2 */
		/* send request to lookup file handle */
		spfsLookupPathRequest *req;
		req = new spfsLookupPathRequest(0, SPFS_LOOKUP_PATH_REQUEST);
		req->setContextPointer(mpireq);
		/* copy path to look up */
		req->setPath(filedes->path.substr(filedes->segstart[filedes->curseg],
                    filedes->seglen[filedes->curseg]).c_str());
		/* get handle for root dir */
		req->setHandle(filedes->handles[filedes->curseg]);
		client->send(req, client->fsNetOut);
		break;
	}
            case FSM_Exit(LOOKUP):
            {
                spfsLookupPathResponse *fsresp = (spfsLookupPathResponse *)resp;
        int i;
        /* save handles found */
        int handle_count = fsresp->getHandle_count();
        for (i = 0; i < handle_count; i++)
        {
            filedes->handles[++filedes->curseg] = fsresp->getHandles(i);
        }
		switch (fsresp->getStatus())
		{
        case SPFS_FOUND :
			filedes->handle = filedes->handles[filedes->curseg];
			/* enter handle in cache */
			filedes->fs->insertDir(filedes->path, filedes->handle);
            if (mpireq->getMode() & MPI_MODE_EXCL)
                FSM_Goto(state, ERREXCL);
            else
            {
			    /* look for metadata in cache */
			    filedes->metaData = filedes->fs->lookupAttr(filedes->handle);
			    if (filedes->metaData.metaHandle == 0)
			    {
				    /* not in cache go to lookup state */
				    FSM_Goto(state, READ_ATTR);
			    }
			    else
			    {
				    /* in cache go to readdir state */
				    FSM_Goto(state, FINISH);
			    }
            }
            break;
        case SPFS_PARTIAL :
			/* set up next lookup */
			FSM_Goto(state, LOOKUP);
            break;
        case SPFS_NOTFOUND :
            if (mpireq->getMode() & MPI_MODE_CREATE &&
                    filedes->curseg == filedes->segcnt - 1)
                /* we found parent dir */
                FSM_Goto(state, CREATE_META);
            else
                /* at this point magically create the file ??? */
                FSM_Goto(state, ERRNF);
            break;
		}
		/* free lookup response message */
        delete fsresp;
		break;
    } /* }}}2 */
	case FSM_Enter(CREATE_META):
    { /* CREATE_META STATE {{{2 */
        int metaserver;
        /* create metadata object */
        spfsCreateRequest *req;
	    /* build a request message */
	    req = new spfsCreateRequest(0, SPFS_CREATE_REQUEST);
	    req->setContextPointer(mpireq);
        /* hash path to meta server number */
		metaserver = filedes->fs->fsHashPath(mpireq->getFileName());
        /* use first handle in range to address server */
		req->setHandleRng(filedes->fs->fsServers(metaserver));
		req->setServerNo(-1);
		client->send(req, client->fsNetOut);
        break;
    }
	case FSM_Exit(CREATE_META):
    {
        spfsCreateResponse *fsresp;
        fsresp = (spfsCreateResponse *)resp;
        filedes->handle = fsresp->getHandle();
        filedes->metaData.metaHandle = filedes->handle;
        FSM_Goto(state, CREATE_DATA);
        delete resp;
        break;
    } /* }}}2 */
	case FSM_Enter(CREATE_DATA):
    { /* CREATE_DATA STATE {{{2 */
		/* send request to create data file */
        spfsCreateRequest *req;
        int numservers;
        int snum;
	    /* build a request message */
	    req = new spfsCreateRequest(0, SPFS_CREATE_REQUEST);
	    req->setContextPointer(mpireq);
        numservers = filedes->fs->fsDefaultNumServers();
	    /* send request to each server */
	    for (snum = 0; snum < numservers; snum++)
	    {
            int dataserver;
            spfsCreateRequest *newreq = (spfsCreateRequest *)req->dup();
            newreq->setServerNo(snum);
            /* randomly select a server from 0 to S-1 */
		    dataserver = filedes->fs->selectServer();
            /* get the bucket range for that server */
            newreq->setHandleRng(filedes->fs->servers[dataserver]);
            /* use first handle of range to address server */
            newreq->setHandle(filedes->fs->servers[dataserver].first);
		    client->send(newreq, client->fsNetOut);
	    }
        /* free req */
        delete req;
        /* indicates how many responses we are waiting for */
        mpireq->setResponses(numservers);
		break;
    }
	case FSM_Exit(CREATE_DATA):
    {
        /* handle create data object response */
        int respcnt;
        spfsCreateResponse *fsresp;
        fsresp = (spfsCreateResponse *)resp;
        respcnt = mpireq->getResponses() - 1;
        /* record the new data object handle in metadata */
        filedes->handles[fsresp->getServerNo()] = fsresp->getHandle();
        /* record new object handle */
        if (respcnt <= 0)
        {
            /* done creating file */
            FSM_Goto(state, WRITE_ATTR);
        }
        else
        {
            /* more responses comming */
            mpireq->setResponses(respcnt);
        }
        /* free response message */
        delete resp;
        break;
    } /* }}}2 */
	case FSM_Enter(WRITE_ATTR):
	{ /* WRITE_ATTR STATE {{{2 */
	    /* send request to write metadata */
		spfsSetAttrRequest *req;
		req = new spfsSetAttrRequest(0, SPFS_SET_ATTR_REQUEST);
		req->setContextPointer(mpireq);
		req->setHandle(filedes->handle);
        req->setMeta(filedes->metaData);
		client->send(req, client->fsNetOut);
		break;
	}
	case FSM_Exit(WRITE_ATTR):
	{
	    spfsSetAttrResponse *fsresp;
		fsresp = (spfsSetAttrResponse *)resp;
        /* do anything ?? */
		FSM_Goto(state, WRITE_DIRENT);
		/* free get attr resp message */
        delete resp;
		break;
	} /* }}}2 */
	case FSM_Enter(WRITE_DIRENT):
	{ /* WRITE_DIRENT STATE {{{2 */
        spfsCreateDirEntRequest *req;
        req = new spfsCreateDirEntRequest(0, SPFS_CREATE_DIR_ENT_REQUEST);
        req->setContextPointer(mpireq);
        /* this addresses the server with the dir on it */
        req->setHandle(filedes->handles[filedes->curseg]);
        /* this is the same handle, the handle of the parent dir */
        /* can we get rid of this field?  WBL */
        req->setParent_handle(filedes->handles[filedes->curseg]);
        /* this is the handle of the new file goes in dirent */
        req->setNew_handle(filedes->metaData.metaHandle);
        /* this is the name of the file */
        req->setEntry(mpireq->getFileName());
		client->send(req, client->fsNetOut);
		break;
	}
	case FSM_Exit(WRITE_DIRENT):
	{
	    spfsCreateDirEntResponse *fsresp;
		fsresp = (spfsCreateDirEntResponse *)resp;
        /* do anything ?? */
		FSM_Goto(state, FINISH);
		/* free create dirent response message */
        delete fsresp;
		break;
	} /* }}}2 */
	case FSM_Enter(READ_ATTR):
	{ /* READ_ATTR STATE {{{2 */
	    /* send request to read metadata */
		spfsGetAttrRequest *req;
		req = new spfsGetAttrRequest(0, SPFS_GET_ATTR_REQUEST);
		req->setContextPointer(mpireq);
		req->setHandle(filedes->handle);
		client->send(req, client->fsNetOut);
		break;
	}
	case FSM_Exit(READ_ATTR):
	{
	    spfsGetAttrResponse *fsresp;
		fsresp = (spfsGetAttrResponse *)resp;
        /* install attributes into attribute cache */
		FSM_Goto(state, FINISH);
		/* free get attr resp message */
        delete fsresp;
		break;
	} /* }}}2 */
	case FSM_Enter(FINISH):
	{ /* FINISH STATE {{{2 */
		/* return descriptor to caller */
		spfsMPIFileOpenResponse *mpiresp;
		mpiresp = new spfsMPIFileOpenResponse(0, SPFS_MPI_FILE_OPEN_RESPONSE);
		mpiresp->setContextPointer(mpireq->contextPointer());
		mpiresp->setFiledes(filedes);
		client->send(mpiresp, client->fsMpiOut);
		/* free original mpi request message */
        delete mpireq;
		break;
	} /* }}}2 */
    default:
        break;
	} /* End of FSM */
    if (mpireq)
        mpireq->setState(state);
}  /* }}}1 */

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
            req->setServerCnt(filedes->metaData.dataHandles.size());
            req->setOffset(filedes->filePtr);
            req->setCount(count);
            req->setDtype(dtype);
            /* send request to each server */
	    for (snum = 0; snum < filedes->metaData.dataHandles.size(); snum++)
	    {
                if (filedes->fs->serverNotUsed(snum, filedes->metaData.dist,
                                               count, dtype))
                    /* don't send if no data on server */
                    continue;
                else
                {
                    spfsReadRequest *newreq = (spfsReadRequest *)req->dup();
                    newreq->setHandle(filedes->metaData.dataHandles[snum]);
                    req->setServerNo(snum);
                    client->send(newreq, client->fsNetOut);
                }
	    }
            /* free req */
            delete req;
            /* indicates how many responses we are waiting for */
            mpireq->setResponses(filedes->metaData.dataHandles.size());
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
	    req->setServer_cnt(filedes->metaData.dataHandles.size());
	    req->setOffset(filedes->filePtr);
	    req->setCount(count);
	    req->setDtype(dtype);
	    /* send request to each server */
	    for (snum = 0; snum < filedes->metaData.dataHandles.size(); snum++)
	    {
		    if (filedes->fs->serverNotUsed(snum, filedes->metaData.dist,
						 count, dtype))
			    /* don't send if no data on server */
			    continue;
		    else
		    {
			    spfsWriteRequest *newreq = (spfsWriteRequest *)req->dup();
			    newreq->setHandle(filedes->metaData.dataHandles[snum]);
			    req->setServer_nr(snum);
			    client->send(newreq, client->fsNetOut);
		    }
	    }
        /* free req */
        delete req;
        /* indicates how many responses we are waiting for */
        mpireq->setResponses(filedes->metaData.dataHandles.size());
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

//*************************************************************************
// Helper functions

void fsParsePath(struct FSOpenFile *fdes)
{
    //bool absolute;
    int size, seg;
    std::string::size_type index;
    size = fdes->path.size();
    index = 0;
    for (seg = 0; index != std::string::npos && seg < MAXSEG; seg++)
    {
        index = fdes->path.find_first_not_of('/', index);
        fdes->segstart[seg] = index;
        fdes->seglen[seg] = size - index;
		index = fdes->path.find_first_of('/', index);
    }
}

/*
 * Local variables:
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 *
 * vim: ts=4 sts=4 sw=4 expandtab foldmethod=marker
 */

