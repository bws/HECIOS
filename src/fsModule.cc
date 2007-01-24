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
	fsProcessMessage(msg);
}

// messages from user/cache
void fsProcessMessage( MPI_open *msg )
{
}

void fsProcessMessage( MPI_close *msg )
{
}

void fsProcessMessage( MPI_read *msg )
{
}

void fsProcessMessage( MPI_readat *msg )
{
}

void fsProcessMessage( MPI_write *msg )
{
}

void fsProcessMessage( MPI_writeat *msg )
{
}

void fsProcessMessage( MPI_seek *msg )
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
void timeout :
