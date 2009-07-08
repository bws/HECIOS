//
// This file is part of Hecios
//
// Copyright (C) 2007,2008,2009 Brad Settlemyer
//
// This file is distributed WITHOUT ANY WARRANTY. See the file 'License.txt'
// for details on this and other legal matters.
//

//
// Network wrapper messages for transporting data from the client to server
//

enum spfsNetworkMessageKinds
{
    SPFS_NETWORK_CLIENT_SEND = 801;
    SPFS_NETWORK_SERVER_SEND = 802;
};

// The abstract base class for all Network Messages
message spfsNetworkMessage
{
    fields:
};

// Message sent over the network from a client 
message spfsNetworkClientSendMessage extends spfsNetworkMessage
{
    fields:
};

// Message sent over the network from a server 
message spfsNetworkServerSendMessage extends spfsNetworkMessage
{
    fields:
};

//
// Local variables:
//  c-indent-level: 4
//  c-basic-offset: 4
// End:
//
// vim: ts=4 sts=4 sw=4 expandtab foldmethod=marker
//
