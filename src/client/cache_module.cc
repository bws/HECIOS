#ifndef CACHE_MODULE_CC
#define CACHE_MODULE_CC

// file cacheModule.cc
// This file implements the cache module message handling
#include <omnetpp.h>
#include <vector>
#include <map>

//#include <cacheModule.h>
#include "cache_entry.h"
#include "mpiio_proto_m.h"
#include "umd_io_trace.h"
#include "lru_simple_cache.h"



using namespace std;


/*typedef struct cacheEntry
{

} *ptrSCacheEntry;

typedef struct cacheLine
{

} *ptrSCacheLine;
*/

//map<int, CacheEntry> systemCache;
//map<int, CacheEntry>::iterator cacheIter;

class cacheModule : public cSimpleModule
{
	public: 
 		/*** Constructor */
		cacheModule() : cSimpleModule(){};	

	protected:
		virtual void initialize();
		virtual void finish();
		virtual void handleMessage(cMessage *msg);
        ~cacheModule();
	
	private:
                // create system cache
                LRUSimpleCache<int, int>* systemCache;

		// Request forward declerations
		void cacheProcessTimer(cMessage *msg );
   		void cacheProcess_mpiFileOpenRequest(mpiFileOpenRequest *msg );
		void cacheProcess_mpiFileCloseRequest(mpiFileCloseRequest *msg );
		void cacheProcess_mpiFileDeleteRequest(mpiFileDeleteRequest *msg );
		void cacheProcess_mpiFileSetSizeRequest(mpiFileSetSizeRequest *msg );
                void cacheProcess_mpiFileGetInfoRequest(mpiFileGetInfoRequest *msg);
                void cacheProcess_mpiFileSetInfoRequest(mpiFileSetInfoRequest *msg);
		void cacheProcess_mpiFilePreallocateRequest( 
											mpiFilePreallocateRequest *msg );
		void cacheProcess_mpiFileGetSizeRequest(mpiFileGetSizeRequest *msg );
		void cacheProcess_mpiFileReadAtRequest(mpiFileReadAtRequest *msg );
		void cacheProcess_mpiFileReadRequest(mpiFileReadRequest *msg );
		void cacheProcess_mpiFileWriteAtRequest(mpiFileWriteAtRequest *msg );
		void cacheProcess_mpiFileWriteRequest(mpiFileWriteRequest *msg );
		
		// Response forward declerations
		void cacheProcess_mpiFileOpenResponse(mpiFileOpenResponse *msg );
		void cacheProcess_mpiFileCloseResponse(mpiFileCloseResponse *msg );
		void cacheProcess_mpiFileDeleteResponse(mpiFileDeleteResponse *msg );
		void cacheProcess_mpiFileSetSizeResponse(mpiFileSetSizeResponse *msg );		void cacheProcess_mpiFilePreallocateResponse( 
				mpiFilePreallocateResponse *msg );
		void cacheProcess_mpiFileGetSizeResponse(mpiFileGetSizeResponse *msg );
		void cacheProcess_mpiFileReadResponse(mpiFileReadResponse *msg );
		void cacheProcess_mpiFileWriteResponse(mpiFileWriteResponse*msg );
	
		// Other funcitons
		void cacheUnknownMessage(cMessage *msg);	
                // funciton declarations
                int cacheLookup(int address, int extent);
                int cacheLookupFileName(const char* fileName);
                int cacheLookupHandle(int handle);
                int cacheAddFileName(const char* fileName);
                int cacheAddHandle(int handle, int extent);
		// delcare gates
		int fsIn;
		int fsOut;
		int appIn;
		int appOut;
};

Define_Module(cacheModule);

//cacheModule::cacheModule()
//{
//}

void cacheModule::initialize()
{
	//define input/output gates
	fsIn = findGate("fsIn");
	fsOut = findGate("fsOut");
	appIn = findGate("appIn");
	appOut = findGate("appOut");
        systemCache = new LRUSimpleCache<int, int>(10);	
}

void cacheModule::finish()
{
	//free(systemCache);
}

cacheModule::~cacheModule()
{
}

void cacheModule::handleMessage(cMessage *msg)
{
	if (msg->isSelfMessage())
    {
		cacheProcessTimer(msg);
	}else
    {
		switch(msg->kind())
		{
		// cases for request handling
		case MPI_FILE_OPEN_REQUEST:  
			cacheProcess_mpiFileOpenRequest((mpiFileOpenRequest*) msg);
			break;
		case MPI_FILE_CLOSE_REQUEST:
			cacheProcess_mpiFileCloseRequest((mpiFileCloseRequest*) msg);
			break;
		case MPI_FILE_DELETE_REQUEST:
			cacheProcess_mpiFileDeleteRequest((mpiFileDeleteRequest*) msg);
			break;
		case MPI_FILE_SET_SIZE_REQUEST:
			cacheProcess_mpiFileSetSizeRequest((mpiFileSetSizeRequest*) msg);
			break;
		case MPI_FILE_PREALLOCATE_REQUEST:
			cacheProcess_mpiFilePreallocateRequest((mpiFilePreallocateRequest*)  msg);
			break;
		case MPI_FILE_GET_SIZE_REQUEST:
			cacheProcess_mpiFileGetSizeRequest((mpiFileGetSizeRequest*) msg);
			break;
	        case MPI_FILE_GET_INFO_REQUEST:
                        cacheProcess_mpiFileGetInfoRequest((mpiFileGetInfoRequest*) msg);
                        break;
                case MPI_FILE_SET_INFO_REQUEST:
                        cacheProcess_mpiFileSetInfoRequest((mpiFileSetInfoRequest*) msg); 
                        break;
	        case MPI_FILE_READ_AT_REQUEST:
			cacheProcess_mpiFileReadAtRequest((mpiFileReadAtRequest*) msg);
                        break;

		case MPI_FILE_READ_REQUEST:
			cacheProcess_mpiFileReadRequest((mpiFileReadRequest*) msg);
			break;
		case MPI_FILE_WRITE_AT_REQUEST:
			cacheProcess_mpiFileWriteAtRequest((mpiFileWriteAtRequest*) msg);
                        break;

		case MPI_FILE_WRITE_REQUEST:
			cacheProcess_mpiFileWriteRequest((mpiFileWriteRequest*) msg);
			break;
		// cases for response handling
		case MPI_FILE_OPEN_RESPONSE:
			cacheProcess_mpiFileOpenResponse((mpiFileOpenResponse*) msg);
			break;
		case MPI_FILE_CLOSE_RESPONSE:
			cacheProcess_mpiFileCloseResponse((mpiFileCloseResponse*) msg);
			break;
		case MPI_FILE_DELETE_RESPONSE:
			cacheProcess_mpiFileDeleteResponse((mpiFileDeleteResponse*) msg);
			break;
		case MPI_FILE_SET_SIZE_RESPONSE:
			cacheProcess_mpiFileSetSizeResponse((mpiFileSetSizeResponse*) msg);
			break;
		case MPI_FILE_PREALLOCATE_RESPONSE:
			cacheProcess_mpiFilePreallocateResponse((mpiFilePreallocateResponse*) msg);
			break;
		case MPI_FILE_GET_SIZE_RESPONSE:
			cacheProcess_mpiFileGetSizeResponse((mpiFileGetSizeResponse*) msg);
			break;
		case MPI_FILE_READ_RESPONSE:
			cacheProcess_mpiFileReadResponse((mpiFileReadResponse*) msg);
			break;
		case MPI_FILE_WRITE_RESPONSE:
			cacheProcess_mpiFileWriteResponse((mpiFileWriteResponse*) msg);
			break;
		default :
			cacheUnknownMessage(msg);
			break;
		}
	}
}

// messages from app/fs - request
// in simplist cases, just send a response for now

void cacheModule::cacheProcess_mpiFileOpenRequest( mpiFileOpenRequest *msg )
{

  // new response message to schedule
  //mpiFileOpenResponse *m = new cMessage("mpiFileOpenResponse");
  // cache search delay goes here
  //int delay = 1;
  // schedule message handling for sometime in future
  //scheduleAt(delay, m);

              cerr << "in here 11111...."
                 << msg->kind() << endl;
    if(cacheLookupFileName(msg->getFileName()))
    {
		// create new message and set message fields
    	mpiFileOpenResponse *m = new mpiFileOpenResponse("mpiFileOpenResponse");
	send(m, appOut);
    }else
    {
        send(msg, fsOut);
    }
}

void cacheModule::cacheProcess_mpiFileCloseRequest( mpiFileCloseRequest *msg )
{
	// look in cache, if found, respond, if not found, sent to fs
    if(cacheLookupHandle(msg->getHandle()))
    {
	// create new message and set message fields
    	mpiFileCloseResponse *m = new 
                            mpiFileCloseResponse("mpiFileCloseResponse");
	send(m, appOut);
    }else
    {
        send(msg, fsOut);
        cacheAddHandle(msg->getHandle(), 1);
    }
}

void cacheModule::cacheProcess_mpiFileDeleteRequest( mpiFileDeleteRequest *msg )
{
    // look in cache, if found, respond, if not found, sent to fs
    if(cacheLookupFileName(msg->getFileName()))
    {
	// create new message and set message fields
        mpiFileDeleteResponse *m = new 
			    mpiFileDeleteResponse("mpiFileDeleteResponse");
	send(m, appOut);
    }else
    {
        send(msg, fsOut);
        cacheAddFileName(msg->getFileName());
    }
}

void cacheModule::cacheProcess_mpiFileSetSizeRequest( mpiFileSetSizeRequest *msg )
{
    // look in cache, if found, respond, if not found, sent to fs
    if(cacheLookupHandle(msg->getHandle()))
    {
    	// create new message and set message fields
        mpiFileSetSizeResponse *m = new 
            		    mpiFileSetSizeResponse("mpiFileSetSizeResponse");
	send(m, appOut);
    }else
    {
        send(msg, fsOut);
        cacheAddHandle(msg->getHandle(), msg->getSize());
    }
}

void cacheModule::cacheProcess_mpiFileGetInfoRequest(mpiFileGetInfoRequest *msg)
{
    // look in cache, if found, respond, if not found, sent to fs
    if(cacheLookupHandle(msg->getHandle()))
    {
		// create new message and set message fields
    	mpiFileGetInfoRequest *m = new 
		    mpiFileGetInfoRequest("mpiFileGetInfoRequest");
	send(m, appOut);
    }else
    {
        send(msg, fsOut);
        cacheAddHandle(msg->getHandle(), 1);
    }



}

void cacheModule::cacheProcess_mpiFileSetInfoRequest(mpiFileSetInfoRequest *msg)
{
    // look in cache, if found, respond, if not found, sent to fs
    if(cacheLookupHandle(msg->getHandle()))
    {
		// create new message and set message fields
    	mpiFileSetInfoRequest *m = new 
		    mpiFileSetInfoRequest("mpiFileSetInfoRequest");
	send(m, appOut);
    }else
    {
        send(msg, fsOut);
        cacheAddHandle(msg->getHandle(), 1);
    }
}




void cacheModule::cacheProcess_mpiFilePreallocateRequest( mpiFilePreallocateRequest *msg )
{
    // look in cache, if found, respond, if not found, sent to fs
    if(cacheLookupHandle(msg->getHandle()))
    {
		// create new message and set message fields
    	mpiFilePreallocateResponse *m = new 
				mpiFilePreallocateResponse("mpiFilePreallocateResponse");
	send(m, appOut);
    }else
    {
        send(msg, fsOut);
        cacheAddHandle(msg->getHandle(),msg->getSize());
    }
}

void cacheModule::cacheProcess_mpiFileGetSizeRequest( mpiFileGetSizeRequest *msg )
{
    // look in cache, if found, respond, if not found, sent to fs
    if(cacheLookupFileName(msg->getFileName()))
    {
		// create new message and set message fields
    	mpiFileGetSizeResponse *m = new 
				mpiFileGetSizeResponse("mpiFileGetSizeResponse");
	send(m, appOut);
    }else
    {
        send(msg, fsOut);
        cacheAddFileName(msg->getFileName());
    }
}

void cacheModule::cacheProcess_mpiFileReadAtRequest( mpiFileReadAtRequest *msg )
{
    // look in cache, if found, respond, if not found, sent to fs
    if(cacheLookupHandle(msg->getHandle()))
    {
		// create new message and set message fields
    	mpiFileReadAtResponse *m = new 
				mpiFileReadAtResponse("mpiFileReadAtResponse");
	send(m, appOut);
    }else
    {
        send(msg, fsOut);
    }
}

void cacheModule::cacheProcess_mpiFileReadRequest( mpiFileReadRequest *msg )
{
    // look in cache, if found, respond, if not found, sent to fs
    if(cacheLookupHandle(msg->getHandle()))
    {
		// create new message and set message fields
    	mpiFileReadResponse *m = new 
				mpiFileReadResponse("mpiFileReadResponse");
	send(m, appOut);
    }else
    {
        send(msg, fsOut);
    }
}

void cacheModule::cacheProcess_mpiFileWriteAtRequest( mpiFileWriteAtRequest *msg )
{
    // look in cache, if found, respond, if not found, sent to fs
    if(cacheLookupHandle(msg->getHandle()))
    {
		// create new message and set message fields
    	mpiFileWriteAtResponse *m = new 
				mpiFileWriteAtResponse("mpiFileWriteAtResponse");
	send(m, appOut);
    }else
    {
        send(msg, fsOut);
    }
}

void cacheModule::cacheProcess_mpiFileWriteRequest( mpiFileWriteRequest *msg )
{
    // look in cache, if found, respond, if not found, sent to fs
    if(cacheLookupHandle(msg->getHandle()))
    {
		// create new message and set message fields
    	mpiFileWriteResponse *m = new 
				mpiFileWriteResponse("mpiFileWriteResponse");
	send(m, appOut);
    }else
    {
        send(msg, fsOut);
    }
}

// messages to fs - responses

void cacheModule::cacheProcess_mpiFileOpenResponse( mpiFileOpenResponse *msg )
{
    // send response through 
    //cacheAddFileName(msg->getFileName());
    send(msg, appOut);
}

void cacheModule::cacheProcess_mpiFileCloseResponse( mpiFileCloseResponse *msg )
{
    // send response through 
    cacheAddHandle(msg->getHandle(),1);
    send(msg, appOut);
    
}

void cacheModule::cacheProcess_mpiFileDeleteResponse( mpiFileDeleteResponse *msg )
{
    // send response through 
    //cacheAdd(msg->);
    send(msg, appOut);
}

void cacheModule::cacheProcess_mpiFileSetSizeResponse( mpiFileSetSizeResponse *msg )
{
    // send response through 
    //cacheAdd(msg->); // exclusive
    send(msg, appOut);
}

void cacheModule::cacheProcess_mpiFilePreallocateResponse( mpiFilePreallocateResponse *msg )
{
    // send response through 
    //cacheAddHandle(msg->getHandle()); // exclusive
    send(msg, appOut);
}

void cacheModule::cacheProcess_mpiFileGetSizeResponse( mpiFileGetSizeResponse *msg )
{
    // send response through 
    //cacheAdd(msg->); // valid
    send(msg, appOut);
}

void cacheModule::cacheProcess_mpiFileReadResponse( mpiFileReadResponse *msg )
{
    // send response through
    //cacheAddHandle(msg->getHandle()); // valid
    send(msg, appOut);
}

void cacheModule::cacheProcess_mpiFileWriteResponse( mpiFileWriteResponse*msg )
{
    // send response through 
    //cacheAdd(msg->); // exclusive
    send(msg, appOut);
}

// defualt message for unknown message type
void cacheModule::cacheUnknownMessage(cMessage *msg)
{
              cerr << "unknown message found"
                 << msg->kind() << endl;
}


// timers from self
void cacheModule::cacheProcessTimer( cMessage *msg )
{
}

int cacheModule::cacheLookupFileName(const char* fileName)
{
    int toReturnFound = 0;  // holds if found and return variable
    // go through cache map and find entry     
    return toReturnFound;
}

int cacheModule::cacheLookupHandle(int handle)
{
    int toReturnFound = 0;  // holds if found and return variable
    //if(systemCache->lookup(handle) != 0) toReturnFound = 1;
	// go through cache map and find entry     
    return toReturnFound;
}

// helper function to perform cache lookup
int cacheModule::cacheLookup(int address, int extent)
{
    int toReturnFound = 0;  // holds if found and return variable
	// go through cache map and find entry     
    return toReturnFound;
}


int cacheModule::cacheAddFileName(const char* fileName)
{
    int toReturnAdded = 0;
	// go through map look for closest entry
	// if entry exists, combine with current,
	// else, just add entry, no eviction right now
	//CacheEntry newEntry = new CacheEntry(address, extent, state);
    //systemCache[address] = newEntry;
    return toReturnAdded;
}

int cacheModule::cacheAddHandle(int handle, int extent)
{
    int toReturnAdded = 0;
    // add to used cache
    systemCache->insert(handle, extent);	
    cerr << "inserting message with handle"
    << handle << endl;
    // if entry exists, combine with current,
    // else, just add entry, no eviction right now
    //CacheEntry newEntry = new CacheEntry(address, extent, 0);
    //systemCache[address] = newEntry;
    return toReturnAdded;
}

// helper function to perform cache add
int cacheAdd(int address, int extent, int state)
{
	// go through map look for closest entry
	// if entry exists, combine with current,
	// else, just add entry, no eviction right now
	// CacheEntry newEntry = new CacheEntry(address, extent, state);
    // systemCache[address] = newEntry;
    return 1;
}


// Main function for testing funcitonality of requests
/*int main()
{

}*/

#endif //CACHE_MODULE_CC

/*
 * Local variables:
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 *
 * vim: ts=8 sts=4 sw=4 expandtab
 */
