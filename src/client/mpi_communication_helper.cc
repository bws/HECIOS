//
// This file is part of Hecios
//
// Copyright (C) 2007,2008,2009 Brad Settlemyer
//
// This file is distributed WITHOUT ANY WARRANTY. See the file 'License.txt'
// for details on this and other legal matters.
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
    //     << "Comm: " << commId
    //     << " Coll Size: " << CommMan::instance().commSize(commId)
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
