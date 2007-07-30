#ifndef CACHE_MODULE_CC
#define CACHE_MODULE_CC

// file cacheModule.cc
// This file implements the cache module message handling
#include <omnetpp.h>
#include <vector>
#include <map>

//#include <cacheModule.h>
#include "cache_entry.h"
#include "mpi_proto_m.h"
#include "umd_io_trace.h"
#include "lru_complex_cache.h"
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
// wheather the cache should have consistency or not
int consistant = 0;

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
                LRUComplexCache* systemCache;

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
                int cacheLookupFileName(const char* fileName);
                /*int cacheLookupHandle(int handle);
                int cacheLookupHandleOffset(int handle, int offset);
                int cacheLookupHandleOffsetExtent(int handle, int offset, 
                                                    int extent);*/
                int cacheAddFileName(const char* fileName);
                int cacheAddHandle(int handle, int extent, int offset);
                void cacheRemoveHandle(int handle);
                void cacheEvict(int handle, int offset, int extent);
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
        systemCache = new LRUComplexCache(300,2000000000);	
}

void cacheModule::finish()
{
	delete systemCache;
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
		/*case CACHE_EVICT_RESPONSE:
                        cacheProcess_cacheEvictResponse((spfsCacheEvictResponse*) msg);
                        break;
                */
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

    // create new message and set message fields
    /*spfsMPIFileOpenResponse *m = new spfsMPIFileOpenResponse("mpiFileOpenResponse",
                                    SPFS_MPI_FILE_OPEN_RESPONSE);
    m->setFileDes(msg->getFileDes());
      
    send(m, appOut);*/
    send(msg, fsOut);
}

void cacheModule::cacheProcess_mpiFileCloseRequest( spfsMPIFileCloseRequest *msg )
{
    // look in cache, if found, respond, if not found, sent to fs
    FSHandle handle = static_cast<FSOpenFile*>(msg->getFileDes())->metaData->handle;
    if(systemCache->findOnlyHandle(handle))
    {
	// create new message and set message fields
    	spfsMPIFileCloseResponse *m = new 
                            spfsMPIFileCloseResponse("mpiFileCloseResponse",
                                    SPFS_MPI_FILE_CLOSE_RESPONSE);
	send(m, appOut);
    }else
    {
        send(msg, fsOut);
        cacheRemoveHandle(handle);        
    }
}

void cacheModule::cacheProcess_mpiFileDeleteRequest( spfsMPIFileDeleteRequest *msg )
{
    // do not look in cache, file will have been closed before deletion,
    // just forward request on
    send(msg, fsOut);
}

void cacheModule::cacheProcess_mpiFileSetSizeRequest( spfsMPIFileSetSizeRequest *msg )
{
    // look in cache, if found, respond, if not found, sent to fs
    FSHandle handle = static_cast<FSOpenFile*>(msg->getFileDes())->metaData->handle;
//    if(cacheLookupHandle(handle))
    if(systemCache->findOnlyHandle(handle))
    {
    	// create new message and set message fields
        spfsMPIFileSetSizeResponse *m = new 
            		    spfsMPIFileSetSizeResponse("mpiFileSetSizeResponse",                               SPFS_MPI_FILE_SET_SIZE_RESPONSE);
	send(m, appOut);
        cacheEvict(handle, msg->getFileSize(),999999);
        if(consistant) send(msg, fsOut);
    }else
    {
        send(msg, fsOut);
        cacheAddHandle(handle, 1, 1);
    }
}

void cacheModule::cacheProcess_mpiFileGetInfoRequest(spfsMPIFileGetInfoRequest *msg)
{
    // look in cache, if found, respond, if not found, sent to fs
    FSHandle handle = static_cast<FSOpenFile*>(msg->getFileDes())->metaData->handle;
//    if(cacheLookupHandle(handle))
    if(systemCache->findOnlyHandle(handle))
    {
		// create new message and set message fields
    	spfsMPIFileGetInfoResponse *m = new 
		    spfsMPIFileGetInfoResponse("mpiFileGetInfoResponse",
                                        SPFS_MPI_FILE_GET_INFO_RESPONSE);
	send(m, appOut);
    }else
    {
        send(msg, fsOut);
        cacheAddHandle(handle, 1, 1);
    }



}

void cacheModule::cacheProcess_mpiFileSetInfoRequest(spfsMPIFileSetInfoRequest *msg)
{
    // look in cache, if found, respond, if not found, sent to fs
    FSHandle handle = static_cast<FSOpenFile*>(msg->getFileDes())->metaData->handle;
//    if(cacheLookupHandle(handle))
    if(systemCache->findOnlyHandle(handle))
    {
		// create new message and set message fields
    	spfsMPIFileSetInfoResponse *m = new 
		    spfsMPIFileSetInfoResponse("mpiFileSetInfoResponse",
                        SPFS_MPI_FILE_SET_INFO_RESPONSE);
	send(m, appOut);
        if(consistant) send(msg, fsOut);
    }else
    {
        send(msg, fsOut);
        cacheAddHandle(handle, 1, 1);
    }
}




void cacheModule::cacheProcess_mpiFilePreallocateRequest( spfsMPIFilePreallocateRequest *msg )
{
    // Now not caching this type of request
    send(msg, fsOut);
    // look in cache, if found, respond, if not found, sent to fs
    /*FSHandle handle = static_cast<FSOpenFile*>(msg->getFileDes())->handle;
        if(systemCache->findOnlyHandle(handle))
    {
		// create new message and set message fields
    	spfsMPIFilePreallocateResponse *m = new 
				spfsMPIFilePreallocateResponse("mpiFilePreallocateResponse",
                                SPFS_MPI_FILE_PREALLOCATE_RESPONSE);
	send(m, appOut);
        if(consistant) send(msg, fsOut);
    }else
    {
        send(msg, fsOut);
        cacheAddHandle(handle,msg->getSize());
    }*/
}

void cacheModule::cacheProcess_mpiFileGetSizeRequest( spfsMPIFileGetSizeRequest *msg )
{
    // look in cache, if found, respond, if not found, sent to fs
    //FSHandle handle = static_cast<FSOpenFile*>(msg->getFiledes())->handle;
    //if(cacheLookupHandle(handle))
    if(cacheLookupFileName(msg->getFileName()))
    {
		// create new message and set message fields
    	spfsMPIFileGetSizeResponse *m = new 
				spfsMPIFileGetSizeResponse("mpiFileGetSizeResponse",
                                SPFS_MPI_FILE_GET_SIZE_RESPONSE);
        // not sure if i should be setting this or not
        // m->setFileSize(msg->size);
        // m->setFileDes(msg->getFileDes());
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
    FSHandle handle = static_cast<FSOpenFile*>(msg->getFileDes())->metaData->handle;

    cerr << "ReadAt request";
    cerr << " handle: " << handle;
    cerr << " off: " << msg->getOffset();
    cerr << " ext: " << msg->getCount() * msg->getDataType() << endl;

    if(systemCache->lookup(handle,
                           msg->getOffset(),
                           msg->getCount() * msg->getDataType()))
    {
		// create new message and set message fields
    	spfsMPIFileReadAtResponse *m = new 
            spfsMPIFileReadAtResponse("mpiFileReadAtResponse",
                                      SPFS_MPI_FILE_READ_AT_RESPONSE);
	send(m, appOut);
    }else
    {
        cacheAddHandle(handle,msg->getOffset(),
                       msg->getCount() * msg->getDataType());
        send(msg, fsOut);
    }
}

void cacheModule::cacheProcess_mpiFileReadRequest( spfsMPIFileReadRequest *msg )
{
    // no read requests, decideing to throw error when see this type
    cerr << "read message found"
               << msg->kind() << endl;
    exit(0);
}

void cacheModule::cacheProcess_mpiFileWriteAtRequest( spfsMPIFileWriteAtRequest *msg )
{
    // look in cache, if found, respond, if not found, sent to fs
    FSHandle handle = static_cast<FSOpenFile*>(msg->getFileDes())->metaData->handle;
    if(systemCache->lookup(handle,msg->getOffset(),
                            msg->getCount() * msg->getDataType()))
    //if(systemCache->findOnlyHandleOffset(handle,msg->getOffset()))
    {
	// create new message and set message fields
    	spfsMPIFileWriteAtResponse *m = new 
				spfsMPIFileWriteAtResponse("mpiFileWriteAtResponse",
                                SPFS_MPI_FILE_WRITE_AT_RESPONSE);
        //m->setFileDes(msg->getFileDes());
	send(m, appOut);
        if(consistant) send(msg, fsOut);
    }else
    {
        cacheAddHandle(handle,msg->getOffset(),msg->getCount());
        send(msg, fsOut);
    }
}

void cacheModule::cacheProcess_mpiFileWriteRequest( spfsMPIFileWriteRequest *msg )
{
    // no write requests, decideing to throw error when see this type
    cerr << "write message found"
               << msg->kind() << endl;
    exit(0);
    // look in cache, if found, respond, if not found, sent to fs
    /*FSHandle handle = static_cast<FSOpenFile*>(msg->getFileDes())->handle;
    // should probably not cache this -------_>>>>>><<<<<_--------
    //    if(cacheLookupHandle(handle))
    if(systemCache->findOnlyHandle(handle))
    {
		// create new message and set message fields
    	spfsMPIFileWriteResponse *m = new 
				spfsMPIFileWriteResponse("mpiFileWriteResponse",
                                SPFS_MPI_FILE_WRITE_RESPONSE);
	send(m, appOut);
        if(consistant) send(msg, fsOut);
    }else
    {
        send(msg, fsOut);
    }*/
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

/*void cacheModule::cacheProcess_cacheEvictRequest(spfsCacheEvictRequest *msg)
{
    FSHandle handle = static_cast<FSOpenFile*>(msg->getFiledes())->handle;
    cacheEvict(handle, ,msg->getOffset(),
                       msg->getCount() * msg->getDataType()));
}


 void cacheModule::cacheProcess_cacheEvictResponse(spfsCacheEvictResponse*mst )
{
    // FSHandle handle = static_cast<FSOpenFile*>(msg->getFiledes())->handle;
    // cacheRemoveHandle(handle);
    send(mst, appOut);
}
*/

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

/*int cacheModule::cacheLookupHandle(int handle)
{
    int toReturnFound = 0;  // holds if found and return variable
    // go through cache map and find entry     
    toReturnFound = systemCache->findOnlyKey(address); 
    if(toReturnFound) // if found in cache, re-insert to indicate use
        cacheAddHandle(address, 1);
    return toReturnFound;
}

// helper function to perform cache lookup
int cacheModule::cacheLookupHandleOffset(int handle, int offset)
{
    int toReturnFound = 0;  // holds if found and return variable
    // go through cache map and find entry     
    toReturnFound = systemCache->findOnlyKeyValue(address, extent);
    if(toReturnFound) // if found in cache, re-insert to indicate use
        cacheAddHandle(address, extent);
    return toReturnFound;
}

int cacheModule::cacheLookupHandleOffsetExtent(int handle, int offset, 
                                                int extent)
{
    int toReturnFound = 0;
    

}*/

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

// would probably include mode in here in the future also
int cacheModule::cacheAddHandle(int handle, int offset, int extent)
{
    int toReturnAdded = 0;
    // add to used cache
    systemCache->insert(handle, offset, extent);	
    cerr << "inserting message with handle"
    << handle << endl;
    toReturnAdded = 1;
    return toReturnAdded;
}

void cacheModule::cacheRemoveHandle(int handle)
{
    systemCache->removeHandle(handle);
}

void cacheModule::cacheEvict(int handle, int offset, int extent)
{
    systemCache->removeOffsetExtent(handle, offset, extent);

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
