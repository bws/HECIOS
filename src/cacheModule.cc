// file cacheModule.cc
// This file implements the cache module message handling
#include <omnetpp.h>
#include <cacheModule.h>
#include <vector>

using namespace std;


/*typedef struct cacheEntry
{

} *ptrSCacheEntry;

typedef struct cacheLine
{

} *ptrSCacheLine;
*/

map<CacheEntry> systemCache;


class cacheModule : public cSimpleModule
{
	protected:
		virtual void initialize();
		virtual void finish();
		virtual void handleMessage(cMessage *msg);
};

Define_Module(cacheModule);

cacheModule::cacheModule()
{
}

void cacheModule::initialize()
{
	//systemCache = malloc(sizeof(CacheEntry)*cacheSize);
}

void cacheModule::finish()
{
	free(systemCache);
}

cacheModule::~cacheModule()
{
}

void cacheModule::handleMessage(cMessage *msg)
{
	if (msg->isSelfMessage())
		cacheProcessTimer(msg)
	else
		switch(msg->kind())
		{
		// cases for request handling
		case mpiFileOpenRequest :
			cacheProcess_mpiFileOpenRequest(msg);
			break;
		case mpiFileOpenRequest :
			cacheProcess_mpiFileOpenRequest(msg);
			break;
		case mpiFileCloseRequest :
			cacheProcess_mpiFileCloseRequest(msg);
			break;
		case mpiFileDeleteRequest :
			cacheProcess_mpiFileDeleteRequest(msg);
			break;
		case mpiFileSetSizeRequest :
			cacheProcess_mpiFileSetSizeRequest(msg);
			break;
		case mpiFilePreallocateRequest :
			cacheProcess_mpiFilePreallocateRequest(msg);
			break;
		case mpiFileGetSizeRequest :
			cacheProcess_mpiFileGetSizeRequest(msg);
			break;
		case mpiFileReadRequest :
			cacheProcess_mpiFileReadRequest(msg);
			break;
		case mpiFileWriteRequest :
			cacheProcess_mpiFileWriteRequest(msg);
			break;
		// cases for response handling
		case mpiFileOpenResponse :
			cacheProcess_mpiFileOpenResponse(msg);
			break;
		case mpiFileOpenResponse :
			cacheProcess_mpiFileOpenResponse(msg);
			break;
		case mpiFileCloseResponse :
			cacheProcess_mpiFileCloseResponse(msg);
			break;
		case mpiFileDeleteResponse :
			cacheProcess_mpiFileDeleteResponse(msg);
			break;
		case mpiFileSetSizeResponse :
			cacheProcess_mpiFileSetSizeResponse(msg);
			break;
		case mpiFilePreallocateResponse :
			cacheProcess_mpiFilePreallocateResponse(msg);
			break;
		case mpiFileGetSizeResponse :
			cacheProcess_mpiFileGetSizeResponse(msg);
			break;
		case mpiFileReadResponse :
			cacheProcess_mpiFileReadResponse(msg);
			break;
		case mpiFileWriteResponse :
			cacheProcess_mpiFileWriteResponse(msg);
			break;
		default :
			cacheUnknownMessage(msg);
			break;
		}
}

// messages from app/fs - request

void cacheProcess_mpiFileOpenRequest( mpiFileOpenRequest *msg )
{
}

void cacheProcess_mpiFileCloseRequest( mpiFileCloseRequest *msg )
{
}

void cacheProcess_mpiFileDeleteRequest( mpiFileDeleteRequest *msg )
{
}

void cacheProcess_mpiFileSetSizeRequest( mpiFileSetSizeRequest *msg )
{
}

void cacheProcess_mpiFilePreallocateRequest( mpiFilePreallocateRequest *msg )
{
}

void cacheProcess_mpiFileGetSizeRequest( mpiFileGetSizeRequest *msg )
{
}

void cacheProcess_mpiFileReadRequest( mpiFileReadRequest *msg )
{
}

void cacheProcess_mpiFileWriteRequest( mpiFileWriteRequest *msg )
{
}

// messages to app/fs - response

void cacheProcess_mpiFileOpenResponse( mpiFileOpenResponse *msg )
{
}

void cacheProcess_mpiFileCloseResponse( mpiFileCloseResponse *msg )
{
}

void cacheProcess_mpiFileDeleteResponse( mpiFileDeleteResponse *msg )
{
}

void cacheProcess_mpiFileSetSizeResponse( mpiFileSetSizeResponse *msg )
{
}

void cacheProcess_mpiFilePreallocateResponse( mpiFilePreallocateResponse *msg )
{
}

void cacheProcess_mpiFileGetSizeResponse( mpiFileGetSizeResponse *msg )
{
}

void cacheProcess_mpiFileReadResponse( mpiFileReadResponse *msg )
{
}

void cacheProcess_mpiFileWriteResponse( mpiFileWriteResponse*msg )
{
}

// defualt message for unknown message type
void cacheUnknownMessage(void *msg)
{
}


// timers from self
void cacheProcessTimer( cMessage *msg )
{
}
