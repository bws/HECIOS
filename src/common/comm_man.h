#ifndef COMM_MAN_H
#define COMM_MAN_H
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
#include <map>
#include <vector>
#include "singleton.h"
#include "pfs_types.h"

/** Type for a collective communicator descriptor */
typedef int Communicator;

/**
 * Communicator Manager (singleton)
 */
class CommMan : public Singleton<CommMan>
{
public:

    /** Enable singleton construction */
    friend class Singleton<CommMan>;

    /** @return The communicator id of MPI_COMM_SELF */
    Communicator commSelf() const;

    /** @return The communicator id of MPI_COMM_WORLD */
    Communicator commWorld() const;

    /** @return The size of comm */
    std::size_t commSize(Communicator comm) const;

    /**
     * Get the rank in a certain communicator
     *  need to be a member of this communicator to use this
     * @param comm The communicator id
     * @param world_rank The rank in MPI_COMM_WORLD
     * @return The rank in comm
     */
    int commRank(Communicator comm, int worldRank) const;

    /** Register a process rank with the communicator manager */
    void registerRank(int rank);

    /** Set the communicator id of MPI_COMM_SELF */
    void setCommSelf(Communicator self);

    /** Set the communicator id of MPI_COMM_WORLD */
    void setCommWorld(Communicator world);

    /**
     * Join a certain communicator
     * @param comm The communicator id
     * @param world_rank The rank in MPI_COMM_WORLD
     * @return The rank in comm
     */
    int joinComm(Communicator comm, int worldRank);

    /** Duplicate comm to comm2 */
    void dupComm(Communicator comm, Communicator comm2);

    /**
     * Translate rank between two communicators
     *  node need to be member of both communicators
     * @param comm1 The 1st communicator
     * @param comm1_rank The rank in 1st communicator
     * @param comm2 The 2nd communicator
     * @return The rank in 2nd communicator, -1 if not in comm2
     */
    int commTrans(int comm1, int comm1_rank, int comm2);

    /** @return true if the communicator exists */
    bool exists(Communicator comm) const;

protected:
    /** Constructor */
    CommMan();

    /** @return true if the world rank exists within the communicator */
    bool worldRankExists(Communicator comm, int worldRank) const;

    /** @return true if the world rank exists within the communicator */
    bool commRankExists(Communicator comm, int commRank) const;

private:
     /** Mapping of old ranks to new ranks */
    typedef std::map<int, int> RankMap;

    /** Map for communicators to rank mappings */
    typedef std::map<Communicator, RankMap> CommunicatorMap;

    /** Copy constructor disabled */
    CommMan(const CommMan& other);

    /** Assignment operator disabled */
    CommMan& operator=(const CommMan& other);

    /** @return a reference to the rank mapping for communicator comm */
    RankMap getRankMap(Communicator comm) const;

    /**
     * Add worldRank to the communicator comm
     *
     * @return the rank value used in the joined communicator
     */
    int addRank(int worldRank, Communicator comm);

    /** Add world rank as new rank to the communicator */
    int addRank(Communicator comm, int worldRank, int newRank);

    /** Rank mappings indexed by the communicator */
    CommunicatorMap rankMapByCommunicator_;

     /** Communicator id for the self communicator */
    Communicator commSelf_;

    /** Communicator id for the all process communicator */
    Communicator commWorld_;
};

/** Process local communicator */
extern Communicator SPFS_COMM_SELF;

/** All process communicator */
extern Communicator SPFS_COMM_WORLD;

#endif
/*
 * Local variables:
 *  indent-tabs-mode: nil
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 *
 * vim: ts=8 sts=4 sw=4 expandtab
 */
