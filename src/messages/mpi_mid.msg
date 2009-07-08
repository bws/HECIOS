//
// This file is part of Hecios
//
// Copyright (C) 2007 Yang Wu
//
// This file is distributed WITHOUT ANY WARRANTY. See the file 'License.txt'
// for details on this and other legal matters.
//

//
// Communication messages between MPI middlewares
//

enum mpiMiddlewareRequestKind
{
    SPFS_MPIMID_REQUEST = 701;
    SPFS_MPIMID_BCAST_REQUEST = 702;
    SPFS_MPIMID_BARRIER_REQUEST = 703;
};

enum mpiMiddlewareResponseKind
{
    SPFS_MPIMID_RESPONSE = 801;
    SPFS_MPIMID_BCAST_RESPONSE = 802;
    SPFS_MPIMID_BARRIER_RESPONSE = 803;
};

message spfsMPIMidRequest
{
    fields:
        int communicator;
};

message spfsMPIMidResponse
{
    fields:
        bool isSuccessful;
};

message spfsMPIMidBcastRequest extends spfsMPIMidRequest
{
    fields:
        int root;
        int parent;
};

message spfsMPIMidBcastResponse extends spfsMPIMidResponse
{
    fields:
};

message spfsMPIMidBarrierRequest extends spfsMPIMidRequest
{
    fields:
}

//
// Local variables:
//  indent-tabs-mode: nil
//  c-indent-level: 4
//  c-basic-offset: 4
// End:
//
// vim: ts=4 sts=4 sw=4 expandtab
//
