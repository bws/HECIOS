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
#include "phtf_io_trace.h"
using namespace std;

bool CommMan::exist(int comm)
{
    bool result;

    if(comm == MPI_COMM_SELF)
    {
        result = true;
    }
    else if(communicators_.find(comm) != communicators_.end())
    {
        result = true;
    }
    else
    {
        result = false;
    }
    return result;
}

bool CommMan::exist(int comm, int rank)
{
    bool result = false;
    
    if(exist(comm))
    {
        if(communicators_[comm]->find(rank) != communicators_[comm]->end())
        {
            result = true;
        }
    }
    return result;
}

int CommMan::joinComm(int comm, int world_rank)
{
    int rank;

    // if join MPI_COMM_SELF, the rank = 0
    if(comm == MPI_COMM_SELF)
    {
        rank = 0;
    }

    else {
        // create the communicator if not exist
        if(!exist(comm))
        {
            communicators_[comm] = new RankPair;
        }
        
        // if join MPI_COMM_WORLD, assign a new rank
        if(comm == MPI_COMM_WORLD)
        {
            int size = commSize(comm);
            
            (*communicators_[comm])[size] = size;
            rank = size;
        }
        
        // if join other communicators
        else
        {
            assert(exist(MPI_COMM_WORLD, world_rank));        
            
            // not a member, assign a new rank
            if(commRank(comm, world_rank) == -1)
            {
                int size = commSize(comm);
                (*communicators_[comm])[size] = world_rank;
                rank = size;
            }
            
            // retuan the rank for member
            else
            {
                rank = commRank(comm, world_rank);
            }
        }
    }

    return rank;
}

size_t CommMan::commSize(int comm)
{
    
    
    size_t size;
    
    if(comm == MPI_COMM_SELF)
    {
        size = 1;
    }
    else if(!exist(comm))
    {
        size = 0;
    }
    else
    {
        size = communicators_[comm]->size();
    }
    
    return size;
}

int CommMan::commRank(int comm, int world_rank)
{
    assert(exist(comm));
    
    RankPair::iterator it;
    int rank;

    if(comm == MPI_COMM_SELF)
    {
        rank = 0;
    }
    else if(comm == MPI_COMM_WORLD)
    {
        rank = world_rank;
    }
    else
    { // search in the communicator
        rank = -1; // not found
        for(it = communicators_[comm]->begin();
            it != communicators_[comm]->end(); it ++)
        {
            if(it->second == world_rank)
            {
                rank = it->first;
            }
        }
    }
    return rank;
}

int CommMan::commTrans(int comm1, int comm1_rank, int comm2)
{
    assert(comm1 != MPI_COMM_SELF);
    assert(exist(comm1, comm1_rank));
        
    int world_rank = comm1_rank;

    if(comm1 != MPI_COMM_WORLD)
    {
        world_rank = (*communicators_[comm1])[comm1_rank];
    }

    return commRank(comm2, world_rank);
}

int CommMan::commWorld()
{
    return commWorld_;
}

void CommMan::commWorld(int world)
{
    commWorld_ = world;
}

int CommMan::commSelf()
{
    return commSelf_;
}

void CommMan::commSelf(int self)
{
    commSelf_ = self;
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
