// file cacheModule.cc
// This file implements the cache module message handling
#include <omnetpp.h>
#include <cacheModule.h>
#include <vector>
#include <map>


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

		int cacheFsIn;
		int cacheFsOut;
		int cacheAppIn;
		int cacheAppOut;
};

Define_Module(cacheModule);

cacheModule::cacheModule()
{
}

void cacheModule::initialize()
{
	//define input/output gates
	cacheFsIn = findGate("cacheFsIn");
	cacheFsOut = findGate("cacheFsOut");
	cacheAppIn = findGate("cacheAppIn");
	cacheAppOut = findGate("cacheAppOut");
	
	
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
// in simplist cases, just send a response for now

void cacheProcess_mpiFileOpenRequest( mpiFileOpenRequest *msg )
{

  // new response message to schedule
  //mpiFileOpenResponse *m = new cMessage("mpiFileOpenResponse");
  // cache search delay goes here
  //int delay = 1;
  // schedule message handling for sometime in future
  //scheduleAt(delay, m);


	// look in cache, if found, respond, if not found, sent to fs
    if(cacheLookup(msg->address, cacheEntry->extent))
    {
		// create new message and set message fields
    	mpiFileOpenResponse *m = new cMessage("mpiFileOpenResponse");
		send(m, cacheAppOut);
    }else
    {
        send(msg, cacheFsOut);
    }
}

void cacheProcess_mpiFileCloseRequest( mpiFileCloseRequest *msg )
{
	// look in cache, if found, respond, if not found, sent to fs
    if(cacheLookup(msg->address, cacheEntry->extent))
    {
		// create new message and set message fields
    	mpiFileCloseResponse *m = new cMessage("mpiFileCloseResponse");
		send(m, cacheAppOut);
    }else
    {
        send(msg, cacheFsOut);
    }
}

void cacheProcess_mpiFileDeleteRequest( mpiFileDeleteRequest *msg )
{
    // look in cache, if found, respond, if not found, sent to fs
    if(cacheLookup(msg->address, msg->extent))
    {
		// create new message and set message fields
    	mpiFileDeleteResponse *m = new cMessage("mpiFileDeleteResponse");
		send(m, cacheAppOut);
    }else
    {
        send(msg, cacheFsOut);
    }
}

void cacheProcess_mpiFileSetSizeRequest( mpiFileSetSizeRequest *msg )
{
    // look in cache, if found, respond, if not found, sent to fs
    if(cacheLookup(msg->address, msg->extent))
    {
		// create new message and set message fields
    	mpiFileSetSizeResponse *m = new cMessage("mpiFileSetSizeResponse");
		send(m, cacheAppOut);
    }else
    {
        send(msg, cacheFsOut);
    }
}

void cacheProcess_mpiFilePreallocateRequest( mpiFilePreallocateRequest *msg )
{
    // look in cache, if found, respond, if not found, sent to fs
    if(cacheLookup(msg->address, msg->extent))
    {
		// create new message and set message fields
    	mpiFilePreallocateResponse *m = new cMessage("mpiFilePreallocateResponse");
		send(m, cacheAppOut);
    }else
    {
        send(msg, cacheFsOut);
    }
}

void cacheProcess_mpiFileGetSizeRequest( mpiFileGetSizeRequest *msg )
{
    // look in cache, if found, respond, if not found, sent to fs
    if(cacheLookup(msg->address, msg->extent))
    {
		// create new message and set message fields
    	mpiFileGetSizeResponse *m = new cMessage("mpiFileGetSizeResponse");
		send(m, cacheAppOut);
    }else
    {
        send(msg, cacheFsOut);
    }
}

void cacheProcess_mpiFileReadRequest( mpiFileReadRequest *msg )
{
    // look in cache, if found, respond, if not found, sent to fs
    if(cacheLookup(msg->address, msg->extent))
    {
		// create new message and set message fields
    	mpiFileReadResponse *m = new cMessage("mpiFileReadResponse");
		send(m, cacheAppOut);
    }else
    {
        send(msg, cacheFsOut);
    }
}

void cacheProcess_mpiFileWriteRequest( mpiFileWriteRequest *msg )
{
    // look in cache, if found, respond, if not found, sent to fs
    if(cacheLookup(msg->address, msg->extent))
    {
		// create new message and set message fields
    	mpiFileWriteResponse *m = new cMessage("mpiFileWriteResponse");
		send(m, cacheAppOut);
    }else
    {
        send(msg, cacheFsOut);
    }
}

// messages to fs - responses

void cacheProcess_mpiFileOpenResponse( mpiFileOpenResponse *msg )
{
    // send response through 
	cacheAdd(msg->offset, msg->extent);
    send(msg, cacheAppOut);
}

void cacheProcess_mpiFileCloseResponse( mpiFileCloseResponse *msg )
{
    // send response through 
	cacheAdd(msg->offset, msg->extent);
    send(msg, cacheAppOut);
    
}

void cacheProcess_mpiFileDeleteResponse( mpiFileDeleteResponse *msg )
{
    // send response through 
	cacheAdd(msg->offset, msg->extent);
    send(msg, cacheAppOut);
}

void cacheProcess_mpiFileSetSizeResponse( mpiFileSetSizeResponse *msg )
{
    // send response through 
	cacheAdd(msg->offset, msg->extent);
    send(msg, cacheAppOut);
}

void cacheProcess_mpiFilePreallocateResponse( mpiFilePreallocateResponse *msg )
{
    // send response through 
	cacheAdd(msg->offset, msg->extent);
    send(msg, cacheAppOut);
}

void cacheProcess_mpiFileGetSizeResponse( mpiFileGetSizeResponse *msg )
{
    // send response through 
	cacheAdd(msg->offset, msg->extent);
    send(msg, cacheAppOut);
}

void cacheProcess_mpiFileReadResponse( mpiFileReadResponse *msg )
{
    // send response through
	cacheAdd(msg->offset, msg->extent);
    send(msg, cacheAppOut);
}

void cacheProcess_mpiFileWriteResponse( mpiFileWriteResponse*msg )
{
    // send response through 
	cacheAdd(msg->offset, msg->extent);
    send(msg, cacheAppOut);
}

// defualt message for unknown message type
void cacheUnknownMessage(void *msg)
{
}


// timers from self
void cacheProcessTimer( cMessage *msg )
{
}

// helper function to perform cache lookup
int cacheLookup(int address, int extent)
{

	// go through cache map and find entry

}

// helper function to perform cache add
int cacheAdd(int address, int extent)
{
	// go through map look for closest entry
	// if entry exists, combine with current,
	// else, just add entry, no eviction right now

}


// Main function for testing funcitonality of requests
int main()
{

}
