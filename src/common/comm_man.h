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
#include "singleton.h"

#define MPI_COMM_WORLD CommMan::instance().commWorld()
#define MPI_COMM_SELF CommMan::instance().commSelf()


/**
 * Communicator Manager (singleton)
 */

class CommMan : public Singleton<CommMan>
{

    typedef std::map<int, int> RankPair;

public:

    /** Enable singleton construction */
    friend class Singleton<CommMan>;
    
    /**
     * Join a certain communicator
     * @param comm The communicator id
     * @param world_rank The rank in MPI_COMM_WORLD
     * @return The rank in comm
     */
    int joinComm(int comm, int world_rank);

    /**
     * Get group size of a communicator
     * @param comm The communicator id
     * @return The size of comm
     */
    size_t commSize(int comm);

    /**
     * Get the rank in a certain communicator
     *  need to be a member of this communicator to use this
     * @param comm The communicator id
     * @param world_rank The rank in MPI_COMM_WORLD
     * @return The rank in comm
     */
    int commRank(int comm, int world_rank);

    /**
     * Translate rank between two communicators
     *  node need to be member of both communicators
     * @param comm1 The 1st communicator
     * @param comm1_rank The rank in 1st communicator
     * @param comm2 The 2nd communicator
     * @return The rank in 2nd communicator, -1 if not in comm2
     */
    int commTrans(int comm1, int comm1_rank, int comm2);

    /** @return The communicator id of MPI_COMM_WORLD */
    int commWorld();

    /** Set the communicator id of MPI_COMM_WORLD */
    void commWorld(int world);

    /** @return The communicator id of MPI_COMM_SELF */
    int commSelf();

    /** Set the communicator id of MPI_COMM_SELF */
    void commSelf(int self);

protected:
    std::map<int, RankPair *> communicators_;// Communicator member map
    bool exist(int comm); // Test if a communicator exists
    bool exist(int comm, int rank); // Test if a communicator member exists
    CommMan(){commWorld_ = 1; commSelf_ = 0;}; // Constructor

private:
    int commWorld_; // Communicator id of MPI_COMM_WORLD
    int commSelf_; // Communicator id of MPI_COMM_SELF
};

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
