//
// This file is part of Hecios
//
// Copyright (C) 2007 Yang Wu
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
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
