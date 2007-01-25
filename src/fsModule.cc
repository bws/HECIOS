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
		switch(msg->kind())
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
}

void fsProcess_mpiFileCloseRequest( mpiFileCloseRequest *msg )
{
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
}

void fsProcess_mpiFileWriteRequest( mpiFileWriteRequest *msg )
{
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
