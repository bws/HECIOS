// file fsModule.cc
#include <omnetpp.h>

class fsModule : public cSimpleModule
{
	protected:
		virtual void initialize();
		virtual void finish();
		virtual void handleMessage(cMessage *msg);
};

Define_Module(fsModule);

fsModule::fsModule()
{
}

void fsModule::initialize()
{
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
		switch(msg->mpiRequestType())
		{
		case mpiFileOpenRequest :
			fsProcess_mpiFileOpenRequest(msg);
			break;
		case mpiFileOpenRequest :
			fsProcess_mpiFileOpenRequest(msg);
			break;
		case mpiFileCloseRequest :
			fsProcess_mpiFileCloseRequest(msg);
			break;
		case mpiFileDeleteRequest :
			fsProcess_mpiFileDeleteRequest(msg);
			break;
		case mpiFileSetSizeRequest :
			fsProcess_mpiFileSetSizeRequest(msg);
			break;
		case mpiFilePreallocateRequest :
			fsProcess_mpiFilePreallocateRequest(msg);
			break;
		case mpiFileGetSizeRequest :
			fsProcess_mpiFileGetSizeRequest(msg);
			break;
		case mpiFileReadRequest :
			fsProcess_mpiFileReadRequest(msg);
			break;
		case mpiFileWriteRequest :
			fsProcess_mpiFileWriteRequest(msg);
			break;
		case fsCreateResponse :
			fsProcess_fsCreateResponse(msg);
			break;
		case fsRemoveResponse :
			fsProcess_fsRemoveResponse(msg);
			break;
		case fsReadResponse :
			fsProcess_fsReadResponse(msg);
			break;
		case fsWriteResponse :
			fsProcess_fsWriteResponse(msg);
			break;
		case fsGetAttrResponse :
			fsProcess_fsGetAttrResponse(msg);
			break;
		case fsSetAttrResponse :
			fsProcess_fsSetAttrResponse(msg);
			break;
		case fsLookupPathResponse :
			fsProcess_fsLookupPathResponse(msg);
			break;
		case fsCreateDirEntResponse :
			fsProcess_fsCreateDirEntResponse(msg);
			break;
		case fsRemoveDirEntResponse :
			fsProcess_fsRemoveDirEntResponse(msg);
			break;
		case fsChangeDirEntResponse :
			fsProcess_fsChangeDirEntResponse(msg);
			break;
		case fsTruncateResponse :
			fsProcess_fsTruncateResponse(msg);
			break;
		case fsMakeDirResponse :
			fsProcess_fsMakeDirResponse(msg);
			break;
		case fsReadDirResponse :
			fsProcess_fsReadDirResponse(msg);
			break;
		case fsWriteCompletionResponse :
			fsProcess_fsWriteCompletionResponse(msg);
			break;
		case fsFlushResponse :
			fsProcess_fsFlushResponse(msg);
			break;
		case fsStatResponse :
			fsProcess_fsStatResponse(msg);
			break;
		case fsListAttrResponse :
			fsProcess_fsListAttrResponse(msg);
			break;
		default :
			fsUnknownMessage(msg);
			break;
		}
}

// messages from user/cache

void fsProcess_mpiFileOpenRequest( mpiFileOpenRequest *msg )
{
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
				req = new fsLookupRequest;
				req->setContextPointer(filedes);
				/* copy path to look up */
				/* get addr for root dir server */
				req->setAddr(filedes->fs.root);
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
				filedes->meta = filedes->fs.lookupMetaData(filedes->handle);
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
				req = new fsGetAttrib;
				req->setContextPointer(filedes);
				req->setHandle(filedes->handle);
				req->setAddr(filedes->fs.map(filedes->handle));
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
				resp = new mpiFileOpenResponse;
				resp->setContextPointer(orig_msg->contextPointer());
				send(resp, fsAppOut);
			}
			break;
	}
}

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
{
	fsReadRequest *req;
	fsHandleSet handles;
	iterator handle;
	/* look for metadata in cache */
	msg->fd->meta = msg->fs.lookupMetaData(msg->fd->handle);
	/* find servers with data */
	fsSetHandles(handles, msg->fd->meta.handles, msg->fd->meta.dist);
	/* send request to each server */
	req = new fsReadRequest;
	req->setContextPointer(msg);
	for (handle = handles.begin(); handle < handles.end(); handle++)
	{
		fsReadRequest newreq = req.dup();
		newreq->setHandle(*handle);
		newreq->setAddr(msg->fs.map(*handle));
		send(newreq, fsNetOut);
	}
}

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
	req = new fsWriteRequest;
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
