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

void fsModle::handleMessage(cMessage *msg)
{
	if (msg->isSelfMessage())
		fsProcessTimer(msg)
	else
		fsProcessMessage(msg);
}

// messages from user/cache

void fsProcessMessage( mpiFileOpenRequest *msg )
{
}

void fsProcessMessage( mpiFileCloseRequest *msg )
{
}

void fsProcessMessage( mpiFileDeleteRequest *msg )
{
}

void fsProcessMessage( mpiFileSetSizeRequest *msg )
{
}

void fsProcessMessage( mpiFilePreallocateRequest *msg )
{
}

void fsProcessMessage( mpiFileGetSizeRequest *msg )
{
}

void fsProcessMessage( mpiFileReadRequest *msg )
{
}

void fsProcessMessage( mpiFileWriteRequest *msg )
{
}

// messages from server/INET

void fsProcessMessage( fsCreateResponse *msg )
{
}

void fsProcessMessage( fsRemoveResponse *msg )
{
}

void fsProcessMessage( fsReadResponse *msg )
{
}

void fsProcessMessage( fsWriteResponse *msg )
{
}

void fsProcessMessage( fsGetAttrResponse *msg )
{
}

void fsProcessMessage( fsSetAttrResponse *msg )
{
}

void fsProcessMessage( fsLookupPathResponse *msg )
{
}

void fsProcessMessage( fsCreateDirEntResponse *msg )
{
}

void fsProcessMessage( fsRemoveDirEntResponse *msg )
{
}

void fsProcessMessage( fsChangeDirEntResponse *msg )
{
}

void fsProcessMessage( fsTruncateResponse *msg )
{
}

void fsProcessMessage( fsMakeDirResponse *msg )
{
}

void fsProcessMessage( fsReadDirResponse *msg )
{
}

void fsProcessMessage( fsWriteCompletionResponse *msg )
{
}

void fsProcessMessage( fsFlushResponse *msg )
{
}

void fsProcessMessage( fsStatResponse *msg )
{
}

void fsProcessMessage( fsListAttrResponse *msg )
{
}

// timers from self
void fsProcessTimer( cMessage *msg )
{
}
