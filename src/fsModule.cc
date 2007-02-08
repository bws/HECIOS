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
		switch(msg->kind())
		{ /* Call message specific hander {{{2 */
		case MPI_FILE_OPEN_REQUEST :
			fsProcess_mpiFileOpenRequest(msg);
			break;
		case MPI_FILE_CLOSE_REQUEST :
			fsProcess_mpiFileCloseRequest(msg);
			break;
		case MPI_FILE_DELETE_REQUEST :
			fsProcess_mpiFileDeleteRequest(msg);
			break;
		case MPI_FILE_SET_SIZE_REQUEST :
			fsProcess_mpiFileSetSizeRequest(msg);
			break;
		case MPI_FILE_PREALLOCATE_REQUEST :
			fsProcess_mpiFilePreallocateRequest(msg);
			break;
		case MPI_FILE_GET_SIZE_REQUEST :
			fsProcess_mpiFileGetSizeRequest(msg);
			break;
		case MPI_FILE_READ_REQUEST :
			fsProcess_mpiFileReadRequest(msg);
			break;
		case MPI_FILE_WRITE_REQUEST :
			fsProcess_mpiFileWriteRequest(msg);
			break;
		case FS_CREATE_RESPONSE :
			fsProcess_fsCreateResponse(msg);
			break;
		case FS_REMOVE_RESPONSE :
			fsProcess_fsRemoveResponse(msg);
			break;
		case FS_READ_RESPONSE :
			fsProcess_fsReadResponse(msg);
			break;
		case FS_WRITE_RESPONSE :
			fsProcess_fsWriteResponse(msg);
			break;
		case FS_GET_ATTR_RESPONSE :
			fsProcess_fsGetAttrResponse(msg);
			break;
		case FS_SET_ATTR_RESPONSE :
			fsProcess_fsSetAttrResponse(msg);
			break;
		case FS_LOOKUP_PATH_RESPONSE :
			fsProcess_fsLookupPathResponse(msg);
			break;
		case FS_CREATE_DIR_ENT_RESPONSE :
			fsProcess_fsCreateDirEntResponse(msg);
			break;
		case FS_REMOVE_DIR_ENT_RESPONSE :
			fsProcess_fsRemoveDirEntResponse(msg);
			break;
		case FS_CHANGE_DIR_ENT_RESPONSE :
			fsProcess_fsChangeDirEntResponse(msg);
			break;
		case FS_TRUNCATE_RESPONSE :
			fsProcess_fsTruncateResponse(msg);
			break;
		case FS_MAKE_DIR_RESPONSE :
			fsProcess_fsMakeDirResponse(msg);
			break;
		case FS_READ_DIR_RESPONSE :
			fsProcess_fsReadDirResponse(msg);
			break;
		case FS_WRITE_COMPLETION_RESPONSE :
			fsProcess_fsWriteCompletionResponse(msg);
			break;
		case FS_FLUSH_RESPONSE :
			fsProcess_fsFlushResponse(msg);
			break;
		case FS_STAT_RESPONSE :
			fsProcess_fsStatResponse(msg);
			break;
		case fs_List_Attr_Response :
			fsProcess_fsListAttrResponse(msg);
			break;
		default :
			fsUnknownMessage(msg);
			break;
		} /* }}}2 */
}

// messages from user/cache

void fsProcess_mpiFileOpenRequest( mpiFileOpenRequest *msg )
{ /* Handle MPI open request {{{1 */
	fsOpenFile *filedes;
	enum {
		INIT = 0;
		LOOKUP = FSM_Steady (1),
		READ_ATTR = FSM_Steady (3),
		FINISH = FSM_Steady (4),
	};
	FSM_Switch(msg->state)
	{
		case FSM_Exit(INIT):
			/* create open file descriptor */
			filedes = new fsOpenFile;
			/* look for dir in cache */
			{
				filedes->fs = msg->fs;
				filedes->handle = filedes->fs.lookupDir(msg->fileName);
				if (filedes->handle == NULL)
				{
					/* not in cache go to lookup state */
					FSM_Goto(filedes->state, LOOKUP);
				}
				else
				{
					/* in cache go to readdir state */
					FSM_Goto(filedes->state, READ_ATTR);
				}
			}
			break;
		case FSM_Enter(LOOKUP):
			/* send request to lookup file handle */
			{
				fsLookupRequest *req;
				req = new fsLookupRequest(0, FS_LOOKUP_REQUEST);
				req->setContextPointer(filedes);
				/* copy path to look up */
				req->setPath(filedes->path);
				/* get handle for root dir */
				req->setHandle(filedes->fs.root);
				send(req, fsNetOut);
			}
			break;
		case FSM_Exit(LOOKUP):
			/* msg is s lookup response */
			filedes = msg->contextPointer();
			if ()
			{
				/* enter handle in cache */
				filedes->handle = msg->handle;
				filedes->fs.insertDir(filedes->path, filedes->handle);
				/* look for metadata in cache */
				filedes->meta = filedes->fs.lookupAttr(filedes->handle);
				if (filedes->meta == NULL)
				{
					/* not in cache go to lookup state */
					FSM_Goto(filedes->state, READ_ATTR);
				}
				else
				{
					/* in cache go to readdir state */
					FSM_Goto(filedes->state, FINISH);
				}
			}
			else
			{
				/* set up next lookup */
			}
			break;
		case FSM_Enter(READ_ATTR):
			/* send request to read metadata */
			{
				fsGetAttrib *req;
				req = new fsGetAttribRequest(0, FS_GET_ATTRIB_REQUEST);
				req->setContextPointer(filedes);
				req->setHandle(filedes->handle);
				send(req, fsNetOut);
			}
			break;
		case FSM_Exit(READ_ATTR):
			filedes = msg->contextPointer();
			FSM_Goto(filedes->state, FINISH);
			break;
		case FSM_Enter(FINISH):
			/* return descriptor to caller */
			{
				mpiFileOpenResponse resp;
				resp = new mpiFileOpenResponse(0, MPI_FILE_OPEN_RESPONSE);
				resp->setContextPointer(orig_msg->contextPointer());
				send(resp, fsAppOut);
			}
			break;
	}
}  /* }}}1 */

void fsProcess_mpiFileCloseRequest( mpiFileCloseRequest *msg )
{
	/* free open file descriptor */
}

void fsProcess_mpiFileDeleteRequest( mpiFileDeleteRequest *msg )
{
}

void fsProcess_mpiFileSetSizeRequest( mpiFileSetSizeRequest *msg )
{
}

void fsProcess_mpiFilePreallocateRequest( mpiFilePreallocateRequest *msg )
{
}

void fsProcess_mpiFileGetSizeRequest( mpiFileGetSizeRequest *msg )
{
}

void fsProcess_mpiFileReadRequest( mpiFileReadRequest *msg )
{ /* process MPI file read request {{{1 */
	fsOpenFile *filedes;
	int count;
	mpiDatatye dtype;
	fsReadRequest *req;
	fsHandleSet handles;
	int snum;

	filedes = msg->getFiledes();
	count = msg->getCount();
	dtype = msg->getDtype();
	/* build a request message */
	req = new fsReadRequest(0, FS_READ_REQUEST);
	req->setContextPointer(msg);
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
} /* }}}1 */

void fsProcess_mpiFileWriteRequest( mpiFileWriteRequest *msg )
{
	fsWriteRequest *req;
	fsHandleSet handles;
	iterator handle;
	/* look for metadata in cache */
	msg->fd->meta = msg->fs.lookupMetaData(msg->fd->handle);
	/* find affected servers using distribution */
	fsSetHandles(handles, msg->fd->meta.handles, msg->fd->meta.dist);
	/* send request to each server */
	req = new fsWriteRequest(0, FS_WRITE_REQUEST);
	req->setContextPointer(msg);
	for (handle = handles.begin(); handle < handles.end(); handle++)
	{
		fsWriteRequest newreq = req.dup();
		newreq->setHandle(*handle);
		newreq->setAddr(msg->fs.map(*handle));
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
