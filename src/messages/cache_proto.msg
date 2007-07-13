//
// Middleware cache consistency/control messages
//

// allow a pointer field in a message 
cplusplus {{
#include <pfs_types.h>

/** MPI Data type */
typedef FSDataType MPIDataType;
}}

class cFSM;
class noncobject MPIDataType;  // this is an int type
class noncobject FileDescriptorPtr;

enum spfsCacheMessageKind
{
    SPFS_CACHE_INVALIDATE_REQUEST = 701;
    SPFS_CACHE_INVALIDATE_RESPONSE = 702;
};

message spfsCacheInvalidateRequest
{
    fields:
        int handle;
        MPIDataType dataType;
        unsigned int count;
}

message spfsCacheInvalidateResponse
{
    fields:
}

//
// Local variables:
//  c-indent-level: 4
//  c-basic-offset: 4
// End:
//
// vim: ts=4 sts=4 sw=4 expandtab foldmethod=marker
//
