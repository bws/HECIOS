// file fsModule.cc
#include <omnetpp.h>

class fsModule : public cSimpleModule
{
	protected:
		virtual void initialize();
		virtual void finish();
		virtual void handleMessage(cMessage *msg);

		int fsMpiOut;
		int fsMpiIn;
		int fsNetOut;
		int fsNetIn;
};

Define_Module(fsModule);

fsModule::fsModule()
{
}

void fsModule::initialize()
{
	fsMpiOut = findGate("fsMpiOut");
	fsMpiIn = findGate("fsMpiIn");
	mpiNetOut = findGate("fsNetOut");
	mpiNetOut = findGate("mfsetIn");
}

void fsModule::finish()
{
}

fsModule::~fsModule()
{
}

void fsModule::handleMessage(cMessage *msg)
{
	if (msg->isSelfMessage())
		fsProcessTimer(msg)
	else
		fsProcessRequest(msg, NULL);
}

void fsProcessRequest(cMessage *req, cMessage *resp)
{ /* Call message specific hander {{{1 */
		switch(req->kind())
		{
		case MPI_FILE_OPEN_REQUEST :
			fsProcess_mpiFileOpenRequest(req, resp);
			break;
		case MPI_FILE_CLOSE_REQUEST :
			fsProcess_mpiFileCloseRequest(req, resp);
			break;
		case MPI_FILE_DELETE_REQUEST :
			fsProcess_mpiFileDeleteRequest(req, resp);
			break;
		case MPI_FILE_SET_SIZE_REQUEST :
			fsProcess_mpiFileSetSizeRequest(req, resp);
			break;
		case MPI_FILE_PREALLOCATE_REQUEST :
			fsProcess_mpiFilePreallocateRequest(req, resp);
			break;
		case MPI_FILE_GET_SIZE_REQUEST :
			fsProcess_mpiFileGetSizeRequest(req, resp);
			break;
		case MPI_FILE_READ_REQUEST :
			fsProcess_mpiFileReadRequest(req, resp);
			break;
		case MPI_FILE_WRITE_REQUEST :
			fsProcess_mpiFileWriteRequest(req, resp);
			break;
		// response messages
		case FS_CREATE_RESPONSE :
		case FS_REMOVE_RESPONSE :
		case FS_READ_RESPONSE :
		case FS_WRITE_RESPONSE :
		case FS_GET_ATTR_RESPONSE :
		case FS_SET_ATTR_RESPONSE :
		case FS_LOOKUP_PATH_RESPONSE :
		case FS_CREATE_DIR_ENT_RESPONSE :
		case FS_REMOVE_DIR_ENT_RESPONSE :
		case FS_CHANGE_DIR_ENT_RESPONSE :
		case FS_TRUNCATE_RESPONSE :
		case FS_MAKE_DIR_RESPONSE :
		case FS_READ_DIR_RESPONSE :
		case FS_WRITE_COMPLETION_RESPONSE :
		case FS_FLUSH_RESPONSE :
		case FS_STAT_RESPONSE :
		case fs_List_Attr_Response :
			fsProcessRequest(req->contextPointer(), req);
			break;
		default :
			fsUnknownMessage(req, resp);
			break;
		}
} /* }}}1 */

// messages from user/cache

void fsProcess_mpiFileOpenRequest( mpiFileOpenRequest *mpireq,
                                   cMessage *resp )
{ /* Handle MPI open request {{{1 */
	fsOpenFile *filedes;
    int state;
	enum {
		INIT = 0;
		LOOKUP = FSM_Steady (1),
        CREATE = FSM_Steady(2),
		READ_ATTR = FSM_Steady (3),
		FINISH = FSM_Steady (4),
		ERRNF = FSM_Steady (5),
		ERREXCL = FSM_Steady (6),
	};
    state = mpireq->getState();
	FSM_Switch(state)
	{
    /* INIT STATE */
	case FSM_Exit(INIT):
	{
		/* create open file descriptor */
		filedes = new fsOpenFile;
		mpireq->filedes = filedes;
		filedes->fs = mpireq->fs;
		/* look for dir in cache */
		filedes->handle = filedes->fs.lookupDir(mpireq->fileName);
		if (filedes->handle == NULL)
		{
			/* not in cache go to lookup state */
			FSM_Goto(state, LOOKUP);
		}
		else
		{
			/* in cache */
            if (mpireq->getMode() & MPI_MODE_EXCL)
                FSM_Goto(state, ERREXCL);
            else
			    FSM_Goto(state, READ_ATTR);
		}
		break;
	}
    /* LOOKUP STATE */
	case FSM_Enter(LOOKUP):
	{
		/* send request to lookup file handle */
		fsLookupRequest *req;
		req = new fsLookupRequest(0, FS_LOOKUP_REQUEST);
		req->setContextPointer(mpireq);
		/* copy path to look up */
		req->setPath(filedes->path);
		/* get handle for root dir */
		req->setHandle(filedes->fs.root);
		send(req, fsNetOut);
		break;
	}
	case FSM_Exit(LOOKUP):
    {
	    fsLookupResponse *fslookupresp;
		fslookupresp = resp;
		filedes = mpireq->filedes;
		switch (fslookpresp->getStatus())
		{
        case FS_FOUND :
			/* enter handle in cache */
			filedes->handle = fslookupresp->handle;
			filedes->fs.insertDir(filedes->path, filedes->handle);
            if (mpireq->getMode() & MPI_MODE_EXCL)
                FSM_Goto(state, ERREXCL);
            else
            {
			    /* look for metadata in cache */
			    filedes->meta = filedes->fs.lookupAttr(filedes->handle);
			    if (filedes->meta == NULL)
			    {
				    /* not in cache go to lookup state */
				    FSM_Goto(state, READ_ATTR);
			    }
			    else
			    {
				    /* in cache go to readdir state */
				    FSM_Goto(state, FINISH);
			    }
			    /* free lookup response message */
                delete fslookupresp;
            }
        case FS_PARTIAL :
			/* set up next lookup */
        case FS_NOTFOUND :
            if (mpireq->getMode() & MPI_MODE_CREATE)
                FSM_Goto(state, CREATE);
            else
                FSM_Goto(state, ERRNF);
		}
		break;
    }
    /* CREATE STATE */
	case FSM_Enter(CREATE):
		/* send request to create file */
		break;
	case FSM_Exit(CREATE):
    {
        fsCreateResponse *fscreateresp;
        fscreateresp = resp;
        if (fscreateresp->getRequestSucceeded())
            FSM_Goto(stat, FINISH);
        else
            FSM_Goto(stat, ERRCREATE);
		break;
    }
    /* READ_ATTR STATE */
	case FSM_Enter(READ_ATTR):
	{
	    /* send request to read metadata */
		fsGetAttrib *req;
		req = new fsGetAttribRequest(0, FS_GET_ATTRIB_REQUEST);
		req->setContextPointer(mpireq);
		req->setHandle(filedes->handle);
		send(req, fsNetOut);
		break;
	}
	case FSM_Exit(READ_ATTR):
	{
	    fsGetAttrResponse *fsgetattrresp;
		fsgetattrresp = resp;
        /* install attributes into attribute cache */
		filedes = mpireq->filedes;
		FSM_Goto(state, FINISH);
		/* free get attr resp message */
        delete fsgetattrresp;
		break;
	}
    /* FINISH STATE */
	case FSM_Enter(FINISH):
	{
		/* return descriptor to caller */
		mpiFileOpenResponse *mpiresp;
		mpiresp = new mpiFileOpenResponse(0, MPI_FILE_OPEN_RESPONSE);
		mpiresp->setContextPointer(mpireq->contextPointer());
		mpiresp->setFiledes(fieldes);
		send(mpiresp, fsAppOut);
		/* free original mpi request message */
        delete mpireq;
		break;
	}
    default:
        break;
	} /* End of FSM */
    if (mpireq)
        mpireq->setState(state);
}  /* }}}1 */

void fsProcess_mpiFileCloseRequest( mpiFileCloseRequest *mpireq,
                                    cMessage *resp )
{
	/* free open file descriptor */
}

void fsProcess_mpiFileDeleteRequest( mpiFileDeleteRequest *mpireq,
                                     cMessage *resp )
{
}

void fsProcess_mpiFileSetSizeRequest( mpiFileSetSizeRequest *mpireq,
                                      cMessage *resp )
{
}

void fsProcess_mpiFilePreallocateRequest( mpiFilePreallocateRequest *mpireq,
                                          cMessage *resp )
{
}

void fsProcess_mpiFileGetSizeRequest( mpiFileGetSizeRequest *mpireq,
                                      cMessage *resp )
{
}

void fsProcess_mpiFileReadRequest( mpiFileReadRequest *mpireq,
                                   cMessage *resp )
{ /* process MPI file read request {{{1 */
	fsOpenFile *filedes;
	int count;
	mpiDatatye dtype;
	fsReadRequest *req;
	fsHandleSet handles;
	int snum;
    int state;
    enum{
        INIT = 0;
        READ = FSM_Steady(1);
    };

    state = mpireq->getSteate();
    switch (state)
    {
    case INIT :
	    filedes = mpireq->filedes;
	    count = mpireq->getCount();
	    dtype = mpireq->getDtype();
        FSM_Goto(state, READ);
        break;
    case FSM_Enter(READ) :
	    /* build a request message */
	    req = new fsReadRequest(0, FS_READ_REQUEST);
	    req->setContextPointer(mpireq);
	    req->setServer_cnt(filedes->meta.handles.size());
	    req->setOffset(filedes->fileptr);
	    req->setCount(count);
	    req->setDtype(dtype);
	    /* send request to each server */
	    for (snum = 0; snum < handles.size(); snum++)
	    {
		    if (filedes->fs->fsServerNotUsed(snum, filedes->meta.dist,
						 count, dtype))
			    /* don't send if no data on server */
			    continue;
		    else
		    {
			    fsReadRequest newreq = req->dup();
			    newreq->setHandle(handles[snum]);
			    req->setServer_nr(snum);
			    send(newreq, fsNetOut);
		    }
	    }
        /* free req */
        /* indicates how many responses we are waiting for */
        mpireq->setResponses(handles.size());
        break;
    case FSM_Exit(READ) :
    {
        int respcnt;
        /* handle read response */
        fsReadResponse *readresp = resp;
        respcnt = mpireq->getResponses() - 1;
        if (respcnt <= 0)
        {
            void *mpi_context;
            /* done with this request */
            mpiFileReadResponse *mpiresp;
            mpiresp = new mpiFileReadResponse(0, MPI_FILE_READ_RESPONSE);
            mpiresp->setContextPointer(mpireq->contextPointer());
            mpiresp->setIsSuccessful(true);
            mpiresp->setBytes_read(readresp->getBytes_read());
            send(mpiresp, fsMpiOut);
        }
        else
        {
            /* more responses comming */
            mpireq->setResponses(respcnt);
        }
        break;
    }
    }
    mpireq->setState(state);
} /* }}}1 */

void fsProcess_mpiFileWriteRequest( mpiFileWriteRequest *mpireq,
                                    cMessage *resp )
{
	fsWriteRequest *req;
	fsHandleSet handles;
	iterator handle;
	/* look for metadata in cache */
	mpireq->fd->meta = mpireq->fs.lookupMetaData(mpireq->fd->handle);
	/* find affected servers using distribution */
	fsSetHandles(handles, mpireq->fd->meta.handles, mpireq->fd->meta.dist);
	/* send request to each server */
	req = new fsWriteRequest(0, FS_WRITE_REQUEST);
	req->setContextPointer(mpireq);
	for (handle = handles.begin(); handle < handles.end(); handle++)
	{
		fsWriteRequest newreq = req.dup();
		newreq->setHandle(*handle);
		newreq->setAddr(mpireq->fs.map(*handle));
		send(newreq, fsNetOut);
	}
}

// messages from server/INET

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

// timers from self
void fsProcessTimer( cMessage *msg )
{
}

//*************************************************************************
// Helper functions

void fsParsePath(mpiFileOpenRequest *msg)
{
	bool absolute;
	int size, index;
	string::size_type start, end;
	size = msg->path.size();
	index = 0;
	start = 0;
	// see if absolutepath and if so skip leading slash
	if (start < size && absolute = (msg->path[start] == '/'))
	{
		start = msg->path.find_first_not_of('/', start);
	}
	// start points to first non-slash
	while(start != string::npos && index < MAXSEGS)
	{
		// find end of segment
		end = msg->path.find_first_of('/', start);
		if (end == string::npos)
		{
			end = size;
		}
		// copy segment
		msg->segment[index++].assign(msg->path, start, end-start);
		// get ready for next segment
		start = msg->path.find_first_not_of('/', end);
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

