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
void fsProcessMessage( fsCreateDirResponse *msg )
{
}

void fsProcessMessage( fsCreateFileResponse *msg )
{
}

void fsProcessMessage( fsLookupPathResponse *msg )
{
}

void fsProcessMessage( fsGetAttrResponse *msg )
{
}

void fsProcessMessage( fsSetAttrResponse *msg )
{
}

void fsProcessMessage( fsReadFileResponse *msg *msg )
{
}

void fsProcessMessage( fsRemoveDirResponse *msg )
{
}

void fsProcessMessage( fsRemoveFileResponse *msg )
{
}

void fsProcessMessage( fsWriteFileResponse *msg )
{
}

// timers from self
void timeout :
