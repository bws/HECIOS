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
#include <cassert>
#include "mpi_communication_helper.h"
#include "comm_man.h"
#include "mpi_proto_m.h"
using namespace std;

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

    // TODO: This implementation is bogus.  It immediately completes all
    // MPI communications
    commUser->completeCommunicationCB(request);
}

void MPICommunicationHelper::performCollective(
    MPICommunicationUserIF* commUser, spfsMPICollectiveRequest* request)
{
    assert(0 != commUser);
    assert(0 != request);

    // Determine the number of existing collective participants
    size_t numParticipants = 1;
    Communicator commId = request->getCommunicator();
    CollectiveCountMap::const_iterator iter =
        numParticipantsByCommunicator_.find(commId);
    if (numParticipantsByCommunicator_.end() != iter)
    {
        numParticipants = 1 + iter->second;
    }
    //cerr << __FILE__ << ":" << __LINE__ << ":"
    //     << "Coll Size: " << CommMan::instance().commSize(commId)
    //     << " Parts Arrived: " << numParticipants << endl;

    // Update the number of participants
    numParticipantsByCommunicator_[commId] = numParticipants;

    // Add this callback to the callback list
    callbacksByCommunicator_[commId].push_back(make_pair(commUser, request));

    // If all members of the communicator have arrived, trigger callback
    if (numParticipants == CommMan::instance().commSize(commId))
    {
        vector<UserCallback>& callbacks = callbacksByCommunicator_[commId];
        for (size_t i = 0; i < callbacks.size(); i++)
        {
            MPICommunicationUserIF* obj = callbacks[i].first;
            spfsMPICollectiveRequest* data = callbacks[i].second;
            obj->completeCommunicationCB(data);
        }

        // Cleanup the maps
        numParticipantsByCommunicator_.erase(commId);
        callbacksByCommunicator_.erase(commId);
    }
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
