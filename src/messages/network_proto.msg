//
// BMI wrapper messages for transporting data from the client to server
//

enum spfsNetworkMessageKinds
{
    SPFS_NETWORK_CLIENT_SEND = 801;
    SPFS_NETWORK_CLIENT_RECV = 802;
    SPFS_NETWORK_SERVER_SEND = 803;
    SPFS_NETWORK_SERVER_RECV = 804;
};

// The abstract base class for all Network Messages
message spfsNetworkMessage
{
    fields:
        unsigned long uniqueId;
};

// Message sent from a client 
message spfsNetworkClientSendMessage extends spfsNetworkMessage
{
};

// Message sent from a server 
message spfsNetworkServerSendMessage extends spfsNetworkMessage
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
