//
// This file is part of Hecios
//
// Copyright (C) 2007,2008,2009 Brad Settlemyer
//
// This file is distributed WITHOUT ANY WARRANTY. See the file 'License.txt'
// for details on this and other legal matters.
//
#include "comm_man.h"
#include <cassert>
#include <set>
using namespace std;

/** Declare the self communicator available for external linkage */
Communicator SPFS_COMM_SELF = CommMan::instance().commSelf();

/** Declare the global communicator available for external linkage */
Communicator SPFS_COMM_WORLD = CommMan::instance().commWorld();

CommMan::CommMan()
    : commSelf_(-1),
      commWorld_(-1)
{
}

Communicator CommMan::commSelf() const
{
    return commSelf_;
}

Communicator CommMan::commWorld() const
{
    return commWorld_;
}

size_t CommMan::commSize(Communicator comm) const
{
    // Have to use a complicated algorithm here because of COMM_SELF
    // We have to count the unique ranks in the communicator
    set<int> uniqueRanks;
    const RankMap& rankMap = getRankMap(comm);
    RankMap::const_iterator first = rankMap.begin();
    RankMap::const_iterator last = rankMap.end();
    while (first != last)
    {
        uniqueRanks.insert(first->second);
        first++;
    }
    return uniqueRanks.size();
}

void CommMan::setCommSelf(Communicator self)
{
    cerr << __FILE__ << ":" << __LINE__ << ":"
         << "DIAGNOSTIC: Setting commSelf to: " << self << endl;
    commSelf_ = self;
    SPFS_COMM_SELF = commSelf_;

    // Create an empty communicator for COMM_SELF
    rankMapByCommunicator_[commSelf_];

}

void CommMan::setCommWorld(Communicator world)
{
    cerr << __FILE__ << ":" << __LINE__ << ":"
         << "DIAGNOSTIC: Setting commWorld to: " << world << endl;
    commWorld_ = world;
    SPFS_COMM_WORLD = commWorld_;

    // Create an empty communicator for COMM_WORLD
    rankMapByCommunicator_[commWorld_];
}

void CommMan::registerRank(int rank)
{
    assert(false == worldRankExists(commWorld_, rank));

    // Add the rank to the all process communicator
    addRank(commWorld_, rank, rank);

    // Add the rank to the self communicator
    addRank(commSelf_, rank, 0);
}

bool CommMan::exists(Communicator comm) const
{
    bool doesExist = false;

    if(0 != rankMapByCommunicator_.count(comm))
    {
        doesExist = true;
    }
    return doesExist;
}

bool CommMan::commRankExists(Communicator comm, int commRank) const
{
    assert(exists(comm));

    // If the rank is less than the size, it exists
    // otherwise, it does not
    const RankMap& rankMap = getRankMap(comm);
    return (size_t(commRank) < rankMap.size());
}

bool CommMan::worldRankExists(Communicator comm, int worldRank) const
{
    bool result = false;
    if (exists(comm))
    {
        const RankMap& rankMap = getRankMap(comm);
        if (0 != rankMap.count(worldRank))
        {
            result = true;
        }
    }
    return result;
}

int CommMan::joinComm(Communicator comm, int worldRank)
{
    assert(comm != commSelf_);
    assert(comm != commWorld_);
    assert(false == worldRankExists(comm, worldRank));
    return addRank(worldRank, comm);
}

int CommMan::commRank(Communicator comm, int worldRank) const
{
    assert(exists(comm));
    assert(worldRankExists(comm, worldRank));
    const RankMap& rankMap = getRankMap(comm);
    RankMap::const_iterator iter = rankMap.find(worldRank);
    return iter->second;
}

void CommMan::dupComm(Communicator comm, Communicator comm2)
{
    assert(exists(comm));
    RankMap map = getRankMap(comm);
    rankMapByCommunicator_[comm2] = map;
}

CommMan::RankMap CommMan::getRankMap(Communicator comm) const
{
    assert(exists(comm));
    CommunicatorMap::const_iterator iter = rankMapByCommunicator_.find(comm);
    assert(iter != rankMapByCommunicator_.end());
    return iter->second;
}

int CommMan::addRank(int worldRank, Communicator comm)
{
    assert(commSelf_ != comm);
    int newRank = rankMapByCommunicator_[comm].size();
    (rankMapByCommunicator_[comm])[worldRank] = newRank;
    return newRank;
}

int CommMan::addRank(Communicator comm, int worldRank, int newRank)
{
    (rankMapByCommunicator_[comm])[worldRank] = newRank;
    return newRank;
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
