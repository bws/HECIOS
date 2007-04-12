//
// User request messages for MPI IO
//

// allow a pointer field in a message 
cplusplus {{
	typedef void *voidPtr;
}}

class noncobject voidPtr;

enum BMIMessageTypes
{
    SPFS_BMI_CLIENT_SEND = 401;
    SPFS_BMI_CLIENT_RECV = 402;
    SPFS_BMI_SERVER_SEND = 403;
    SPFS_BMI_SERVER_RECV = 404;
};

// The abstract base class for all BMI Messages
message BMIMessage
{
    fields:
        unsigned long uniqueId;
};

// Message sent from a BMI client 
message BMIClientSendMessage extends BMIMessage
{
};

// Message sent from a BMI server 
message BMIServerSendMessage extends BMIMessage
{
};

//
// Local variables:
//  c-indent-level: 4
//  c-basic-offset: 4
// End:
//
// vim: ts=4 sts=4 sw=4 expandtab foldmethod=marker
//
