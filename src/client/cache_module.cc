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
#include "cache_module.h"


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
                LRUSimpleCache* systemCache;

		// Request forward declerations
		void cacheProcessTimer(cMessage *msg );
   		void cacheProcess_mpiFileOpenRequest(spfsMPIFileOpenRequest *msg );
		void cacheProcess_mpiFileCloseRequest(spfsMPIFileCloseRequest *msg );
		void cacheProcess_mpiFileDeleteRequest(spfsMPIFileDeleteRequest *msg );
		void cacheProcess_mpiFileSetSizeRequest(spfsMPIFileSetSizeRequest *msg );
                void cacheProcess_mpiFileGetInfoRequest(spfsMPIFileGetInfoRequest *msg);
                void cacheProcess_mpiFileSetInfoRequest(spfsMPIFileSetInfoRequest *msg);
		void cacheProcess_mpiFilePreallocateRequest( 
											spfsMPIFilePreallocateRequest *msg );
		void cacheProcess_mpiFileGetSizeRequest(spfsMPIFileGetSizeRequest *msg );
		void cacheProcess_mpiFileReadAtRequest(spfsMPIFileReadAtRequest *msg );
		void cacheProcess_mpiFileReadRequest(spfsMPIFileReadRequest *msg );
		void cacheProcess_mpiFileWriteAtRequest(spfsMPIFileWriteAtRequest *msg );
		void cacheProcess_mpiFileWriteRequest(spfsMPIFileWriteRequest *msg );
		
		// Response forward declerations
		void cacheProcess_mpiFileOpenResponse(spfsMPIFileOpenResponse *msg );
		void cacheProcess_mpiFileCloseResponse(spfsMPIFileCloseResponse *msg );
		void cacheProcess_mpiFileDeleteResponse(spfsMPIFileDeleteResponse *msg );
    void cacheProcess_mpiFileSetSizeResponse(spfsMPIFileSetSizeResponse *msg );
    void cacheProcess_mpiFilePreallocateResponse( 
        spfsMPIFilePreallocateResponse *msg );
		void cacheProcess_mpiFileGetSizeResponse(spfsMPIFileGetSizeResponse *msg );
		void cacheProcess_mpiFileReadAtResponse(spfsMPIFileReadAtResponse *msg );
		void cacheProcess_mpiFileReadResponse(spfsMPIFileReadResponse *msg );
		void cacheProcess_mpiFileWriteResponse(spfsMPIFileWriteResponse*msg );
		void cacheProcess_mpiFileWriteAtResponse(spfsMPIFileWriteAtResponse*msg );
	
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
        systemCache = new LRUSimpleCache(10);	
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
    {
		cacheProcessTimer(msg);
	}else
    {
		switch(msg->kind())
		{
		// cases for request handling
		case SPFS_MPI_FILE_OPEN_REQUEST:  
			cacheProcess_mpiFileOpenRequest((spfsMPIFileOpenRequest*) msg);
			break;
		case SPFS_MPI_FILE_CLOSE_REQUEST:
			cacheProcess_mpiFileCloseRequest((spfsMPIFileCloseRequest*) msg);
			break;
		case SPFS_MPI_FILE_DELETE_REQUEST:
			cacheProcess_mpiFileDeleteRequest((spfsMPIFileDeleteRequest*) msg);
			break;
		case SPFS_MPI_FILE_SET_SIZE_REQUEST:
			cacheProcess_mpiFileSetSizeRequest((spfsMPIFileSetSizeRequest*) msg);
			break;
		case SPFS_MPI_FILE_PREALLOCATE_REQUEST:
			cacheProcess_mpiFilePreallocateRequest((spfsMPIFilePreallocateRequest*)  msg);
			break;
		case SPFS_MPI_FILE_GET_SIZE_REQUEST:
			cacheProcess_mpiFileGetSizeRequest((spfsMPIFileGetSizeRequest*) msg);
			break;
	        case SPFS_MPI_FILE_GET_INFO_REQUEST:
                        cacheProcess_mpiFileGetInfoRequest((spfsMPIFileGetInfoRequest*) msg);
                        break;
                case SPFS_MPI_FILE_SET_INFO_REQUEST:
                        cacheProcess_mpiFileSetInfoRequest((spfsMPIFileSetInfoRequest*) msg); 
                        break;
	        case SPFS_MPI_FILE_READ_AT_REQUEST:
			cacheProcess_mpiFileReadAtRequest((spfsMPIFileReadAtRequest*) msg);
                        break;

		case SPFS_MPI_FILE_READ_REQUEST:
			cacheProcess_mpiFileReadRequest((spfsMPIFileReadRequest*) msg);
			break;
		case SPFS_MPI_FILE_WRITE_AT_REQUEST:
			cacheProcess_mpiFileWriteAtRequest((spfsMPIFileWriteAtRequest*) msg);
                        break;

		case SPFS_MPI_FILE_WRITE_REQUEST:
			cacheProcess_mpiFileWriteRequest((spfsMPIFileWriteRequest*) msg);
			break;
		// cases for response handling
		case SPFS_MPI_FILE_OPEN_RESPONSE:
			cacheProcess_mpiFileOpenResponse((spfsMPIFileOpenResponse*) msg);
			break;
		case SPFS_MPI_FILE_CLOSE_RESPONSE:
			cacheProcess_mpiFileCloseResponse((spfsMPIFileCloseResponse*) msg);
			break;
		case SPFS_MPI_FILE_DELETE_RESPONSE:
			cacheProcess_mpiFileDeleteResponse((spfsMPIFileDeleteResponse*) msg);
			break;
		case SPFS_MPI_FILE_SET_SIZE_RESPONSE:
			cacheProcess_mpiFileSetSizeResponse((spfsMPIFileSetSizeResponse*) msg);
			break;
		case SPFS_MPI_FILE_PREALLOCATE_RESPONSE:
			cacheProcess_mpiFilePreallocateResponse((spfsMPIFilePreallocateResponse*) msg);
			break;
		case SPFS_MPI_FILE_GET_SIZE_RESPONSE:
			cacheProcess_mpiFileGetSizeResponse((spfsMPIFileGetSizeResponse*) msg);
			break;
		case SPFS_MPI_FILE_READ_RESPONSE:
			cacheProcess_mpiFileReadResponse((spfsMPIFileReadResponse*) msg);
			break;
		case SPFS_MPI_FILE_READ_AT_RESPONSE:
			cacheProcess_mpiFileReadAtResponse((spfsMPIFileReadAtResponse*) msg);
			break;
		case SPFS_MPI_FILE_WRITE_RESPONSE:
			cacheProcess_mpiFileWriteResponse((spfsMPIFileWriteResponse*) msg);
			break;
		case SPFS_MPI_FILE_WRITE_AT_RESPONSE:
			cacheProcess_mpiFileWriteAtResponse((spfsMPIFileWriteAtResponse*) msg);
			break;
		default :
			cacheUnknownMessage(msg);
			break;
		}
	}
}

// messages from app/fs - request
// in simplist cases, just send a response for now

void cacheModule::cacheProcess_mpiFileOpenRequest( spfsMPIFileOpenRequest *msg )
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
    	spfsMPIFileOpenResponse *m = new spfsMPIFileOpenResponse("mpiFileOpenResponse",
                                    SPFS_MPI_FILE_OPEN_RESPONSE);
        m->setFileDes(msg->getFileDes());
        
	send(m, appOut);
    }else
    {
        send(msg, fsOut);
    }
}

void cacheModule::cacheProcess_mpiFileCloseRequest( spfsMPIFileCloseRequest *msg )
{
    // look in cache, if found, respond, if not found, sent to fs
    FSHandle handle = static_cast<FSOpenFile*>(msg->getFileDes())->handle;
    if(cacheLookupHandle(handle))
    {
	// create new message and set message fields
    	spfsMPIFileCloseResponse *m = new 
                            spfsMPIFileCloseResponse("mpiFileCloseResponse",
                                    SPFS_MPI_FILE_CLOSE_RESPONSE);
	send(m, appOut);
    }else
    {
        send(msg, fsOut);
        cacheAddHandle(handle, 1);
    }
}

void cacheModule::cacheProcess_mpiFileDeleteRequest( spfsMPIFileDeleteRequest *msg )
{
    // look in cache, if found, respond, if not found, sent to fs
    if(cacheLookupFileName(msg->getFileName()))
    {
	// create new message and set message fields
        spfsMPIFileDeleteResponse *m = new 
			    spfsMPIFileDeleteResponse("mpiFileDeleteResponse",
                                                SPFS_MPI_FILE_DELETE_RESPONSE);
        //m->kind = 
	send(m, appOut);
    }else
    {
        send(msg, fsOut);
        cacheAddFileName(msg->getFileName());
    }
}

void cacheModule::cacheProcess_mpiFileSetSizeRequest( spfsMPIFileSetSizeRequest *msg )
{
    // look in cache, if found, respond, if not found, sent to fs
    FSHandle handle = static_cast<FSOpenFile*>(msg->getFileDes())->handle;
    if(cacheLookupHandle(handle))
    {
    	// create new message and set message fields
        spfsMPIFileSetSizeResponse *m = new 
            		    spfsMPIFileSetSizeResponse("mpiFileSetSizeResponse",                               SPFS_MPI_FILE_SET_SIZE_RESPONSE);
	send(m, appOut);
    }else
    {
        send(msg, fsOut);
        cacheAddHandle(handle, msg->getSize());
    }
}

void cacheModule::cacheProcess_mpiFileGetInfoRequest(spfsMPIFileGetInfoRequest *msg)
{
    // look in cache, if found, respond, if not found, sent to fs
    FSHandle handle = static_cast<FSOpenFile*>(msg->getFileDes())->handle;
    if(cacheLookupHandle(handle))
    {
		// create new message and set message fields
    	spfsMPIFileGetInfoResponse *m = new 
		    spfsMPIFileGetInfoResponse("mpiFileGetInfoResponse",
                                        SPFS_MPI_FILE_GET_INFO_RESPONSE);
	send(m, appOut);
    }else
    {
        send(msg, fsOut);
        cacheAddHandle(handle, 1);
    }



}

void cacheModule::cacheProcess_mpiFileSetInfoRequest(spfsMPIFileSetInfoRequest *msg)
{
    // look in cache, if found, respond, if not found, sent to fs
    FSHandle handle = static_cast<FSOpenFile*>(msg->getFileDes())->handle;
    if(cacheLookupHandle(handle))
    {
		// create new message and set message fields
    	spfsMPIFileSetInfoResponse *m = new 
		    spfsMPIFileSetInfoResponse("mpiFileSetInfoResponse",
                        SPFS_MPI_FILE_SET_INFO_RESPONSE);
	send(m, appOut);
    }else
    {
        send(msg, fsOut);
        cacheAddHandle(handle, 1);
    }
}




void cacheModule::cacheProcess_mpiFilePreallocateRequest( spfsMPIFilePreallocateRequest *msg )
{
    // look in cache, if found, respond, if not found, sent to fs
    FSHandle handle = static_cast<FSOpenFile*>(msg->getFileDes())->handle;
    if(cacheLookupHandle(handle))
    {
		// create new message and set message fields
    	spfsMPIFilePreallocateResponse *m = new 
				spfsMPIFilePreallocateResponse("mpiFilePreallocateResponse",
                                SPFS_MPI_FILE_PREALLOCATE_RESPONSE);
	send(m, appOut);
    }else
    {
        send(msg, fsOut);
        cacheAddHandle(handle,msg->getSize());
    }
}

void cacheModule::cacheProcess_mpiFileGetSizeRequest( spfsMPIFileGetSizeRequest *msg )
{
    // look in cache, if found, respond, if not found, sent to fs
    if(cacheLookupFileName(msg->getFileName()))
    {
		// create new message and set message fields
    	spfsMPIFileGetSizeResponse *m = new 
				spfsMPIFileGetSizeResponse("mpiFileGetSizeResponse",
                                SPFS_MPI_FILE_GET_SIZE_RESPONSE);
        // not sure if i should be setting this or not
        //m->setFileSize(msg->size);
	send(m, appOut);
    }else
    {
        send(msg, fsOut);
        cacheAddFileName(msg->getFileName());
    }
}

void cacheModule::cacheProcess_mpiFileReadAtRequest( spfsMPIFileReadAtRequest *msg )
{
    // look in cache, if found, respond, if not found, sent to fs
    FSHandle handle = static_cast<FSOpenFile*>(msg->getFileDes())->handle;
    // should probably not cache this ----------_>>>><<<<<_---------------
    if(cacheLookupHandle(handle))
    {
		// create new message and set message fields
    	spfsMPIFileReadAtResponse *m = new 
				spfsMPIFileReadAtResponse("mpiFileReadAtResponse",
                                SPFS_MPI_FILE_READ_AT_RESPONSE);
	send(m, appOut);
    }else
    {
        send(msg, fsOut);
    }
}

void cacheModule::cacheProcess_mpiFileReadRequest( spfsMPIFileReadRequest *msg )
{
    // look in cache, if found, respond, if not found, sent to fs
    FSHandle handle = static_cast<FSOpenFile*>(msg->getFileDes())->handle;
    //should probably not cache this --------_>>>>><<<<<_---------
    if(cacheLookupHandle(handle))
    {
		// create new message and set message fields
    	spfsMPIFileReadResponse *m = new 
				spfsMPIFileReadResponse("mpiFileReadResponse",
                                SPFS_MPI_FILE_READ_RESPONSE);
	send(m, appOut);
    }else
    {
        send(msg, fsOut);
    }
}

void cacheModule::cacheProcess_mpiFileWriteAtRequest( spfsMPIFileWriteAtRequest *msg )
{
    // look in cache, if found, respond, if not found, sent to fs
    FSHandle handle = static_cast<FSOpenFile*>(msg->getFileDes())->handle;
    // should probably not cache this or lookup in cache ------_>>>><<<_----
    if(cacheLookupHandle(handle))
    {
		// create new message and set message fields
    	spfsMPIFileWriteAtResponse *m = new 
				spfsMPIFileWriteAtResponse("mpiFileWriteAtResponse",
                                SPFS_MPI_FILE_WRITE_AT_RESPONSE);
	send(m, appOut);
    }else
    {
        send(msg, fsOut);
    }
}

void cacheModule::cacheProcess_mpiFileWriteRequest( spfsMPIFileWriteRequest *msg )
{
    // look in cache, if found, respond, if not found, sent to fs
    FSHandle handle = static_cast<FSOpenFile*>(msg->getFileDes())->handle;
    // should probably not cache this -------_>>>>>><<<<<_--------
    if(cacheLookupHandle(handle))
    {
		// create new message and set message fields
    	spfsMPIFileWriteResponse *m = new 
				spfsMPIFileWriteResponse("mpiFileWriteResponse",
                                SPFS_MPI_FILE_WRITE_RESPONSE);
	send(m, appOut);
    }else
    {
        send(msg, fsOut);
    }
}

// messages to fs - responses

void cacheModule::cacheProcess_mpiFileOpenResponse( spfsMPIFileOpenResponse *msg )
{
    // send response through 
    //cacheAddFileName(msg->getFileName());
    send(msg, appOut);
}

void cacheModule::cacheProcess_mpiFileCloseResponse( spfsMPIFileCloseResponse *msg )
{
    // send response through 
    //cacheAddHandle(handle,1);
    send(msg, appOut);
    
}

void cacheModule::cacheProcess_mpiFileDeleteResponse( spfsMPIFileDeleteResponse *msg )
{
    // send response through 
    //cacheAdd(msg->);
    send(msg, appOut);
}

void cacheModule::cacheProcess_mpiFileSetSizeResponse( spfsMPIFileSetSizeResponse *msg )
{
    // send response through 
    //cacheAdd(msg->); // exclusive
    send(msg, appOut);
}

void cacheModule::cacheProcess_mpiFilePreallocateResponse( spfsMPIFilePreallocateResponse *msg )
{
    // send response through 
    //cacheAddHandle(msg->getHandle()); // exclusive
    send(msg, appOut);
}

void cacheModule::cacheProcess_mpiFileGetSizeResponse( spfsMPIFileGetSizeResponse *msg )
{
    // send response through 
    //cacheAdd(msg->); // valid
    send(msg, appOut);
}

void cacheModule::cacheProcess_mpiFileReadResponse( spfsMPIFileReadResponse *msg )
{
    // send response through
    //cacheAddHandle(msg->getHandle()); // valid
    send(msg, appOut);
}


void cacheModule::cacheProcess_mpiFileReadAtResponse( spfsMPIFileReadAtResponse *msg )
{
    // send response through
    //cacheAddHandle(msg->getHandle()); // valid
    send(msg, appOut);
}

void cacheModule::cacheProcess_mpiFileWriteResponse( spfsMPIFileWriteResponse*msg )
{
    // send response through 
    //cacheAdd(msg->); // exclusive
    send(msg, appOut);
}

void cacheModule::cacheProcess_mpiFileWriteAtResponse( spfsMPIFileWriteAtResponse*msg )
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

int cacheModule::cacheLookupHandle(int address)
{
    int toReturnFound = 0;  // holds if found and return variable
    // go through cache map and find entry     
    toReturnFound = systemCache->findOnlyKey(address); 
    return toReturnFound;
}

// helper function to perform cache lookup
int cacheModule::cacheLookup(int address, int extent)
{
    int toReturnFound = 0;  // holds if found and return variable
    // go through cache map and find entry     
    toReturnFound = systemCache->findOnlyKeyValue(address, extent);
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

int cacheModule::cacheAddHandle(int handle, int extent) // would probably include mode in here in the future also
{
    int toReturnAdded = 0;
    // add to used cache
    systemCache->insert(handle, extent);	
    cerr << "inserting message with handle"
    << handle << endl;
    return toReturnAdded;
}

// helper function to perform cache add
/*int cacheAdd(int address, int extent, int state)
{
	// go through map look for closest entry
	// if entry exists, combine with current,
	// else, just add entry, no eviction right now
	// CacheEntry newEntry = new CacheEntry(address, extent, state);
    // systemCache[address] = newEntry;
    return 1;
}*/


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
