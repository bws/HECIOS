//
// This file is part of Hecios
//
// Copyright (C) 2008 Brad Settlemyer
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
#include "mpi_communication_helper.h"
#include "mpi_proto_m.h"

MPICommunicationUserIF::~MPICommunicationUserIF()
{
}

MPICommunicationHelper::MPICommunicationHelper()
{
}

MPICommunicationHelper::~MPICommunicationHelper()
{
}

void MPICommunicationHelper::performCommunication(
    MPICommunicationUserIF* commUser,
    spfsMPIRequest* request,
    int otherRank)
{
    assert(0 != commUser);
    assert(0 != request);
    
    // FIXME: This implementation is bogus.  It immediately completes all
    // MPI communications
    commUser->completeCommunicationCB(request);
}

void MPICommunicationHelper::performCollective(
    MPICommunicationUserIF* commUser, spfsMPICollectiveRequest* request)
{
    assert(0 != commUser);
    assert(0 != request);
    
    // FIXME: This implementation is bogus.  It immediately completes all
    // MPI communications
    commUser->completeCommunicationCB(request);
}

/*
 * Local variables:
 *  indent-tabs-mode: nil
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 *
 * vim: ts=8 sts=4 sw=4 expandtab
 */
