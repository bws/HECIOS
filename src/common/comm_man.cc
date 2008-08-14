//
// This file is part of Hecios
//
// Copyright (C) 2008 Yang Wu
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
#include "comm_man.h"
#include <cassert>
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
    size_t size = 1;

    if (commSelf_ != comm)
    {
        assert(exists(comm));
        const RankMap& rankMap = getRankMap(comm);
        size = rankMap.size();
    }
    return size;
}

void CommMan::setCommSelf(Communicator self)
{
    cerr << __FILE__ << ":" << __LINE__ << ":"
         << "DIAGNOSTIC: Setting commSelf to: " << self << endl;
    commSelf_ = self;
    SPFS_COMM_SELF = commSelf_;
}

void CommMan::setCommWorld(Communicator world)
{
    cerr << __FILE__ << ":" << __LINE__ << ":"
         << "DIAGNOSTIC: Setting commWorld to: " << world << endl;
    commWorld_ = world;
    SPFS_COMM_WORLD = commWorld_;

    // Create an empty communicator for commWorld
    rankMapByCommunicator_[commWorld_];
}

void CommMan::registerRank(int rank)
{
    assert(false == worldRankExists(commWorld_, rank));
    // Add the rank to the all process communicator
    addRank(rank, commWorld_);
}

bool CommMan::exists(Communicator comm) const
{
    bool doesExist = false;

    if(commSelf_ == comm ||
       0 != rankMapByCommunicator_.count(comm))
    {
        doesExist = true;
    }
    return doesExist;
}

bool CommMan::commRankExists(Communicator comm, int commRank) const
{
    assert(exists(comm));

    const RankMap& rankMap = getRankMap(comm);

    // If the rank is less than the size, it exists
    // otherwise, it does not
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
    int rank = -1;
    if (commSelf_ == comm)
    {
        rank = 0;
    }
    else if (commWorld_ == comm)
    {
        rank = worldRank;
    }
    else
    {
        assert(exists(comm));
        assert(worldRankExists(comm, worldRank));
        const RankMap& rankMap = getRankMap(comm);
        RankMap::const_iterator iter = rankMap.find(worldRank);
        rank = iter->second;
    }
    return rank;
}

void CommMan::dupComm(Communicator comm, Communicator comm2)
{
    assert(exists(comm));
    assert(!exists(comm2));
    RankMap map = getRankMap(comm);
    rankMapByCommunicator_[comm2] = map;
}

CommMan::RankMap CommMan::getRankMap(Communicator comm) const
{
    assert(commSelf_ != comm);
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


/*
 * Local variables:
 *  indent-tabs-mode: nil
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 *
 * vim: ts=8 sts=4 sw=4 expandtab
 */
