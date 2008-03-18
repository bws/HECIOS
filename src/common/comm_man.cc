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

using namespace std;

bool CommMan::exist(int comm)
{
    if(communicators_.find(comm) != communicators_.end())
        return true;
    else
        return false;
}

bool CommMan::exist(int comm, int rank)
{
    if(exist(comm))
    {
        if(communicators_[comm]->find(rank) != communicators_[comm]->end())
            return true;
    }
    return false;
}

int CommMan::joinComm(int comm, int wrank)
{
    if(comm == MPI_COMM_SELF)return 0;
    
    if(!exist(comm))
        communicators_[comm] = new RankPair;

    int size = commSize(comm);
    (*communicators_[comm])[size] = comm == MPI_COMM_WORLD ? size : wrank;

    return size;
}

int CommMan::commSize(int comm)
{
    if(comm == MPI_COMM_SELF)return 1;
    
    if(exist(comm))
        return (int)communicators_[comm]->size();
    return 0;
}

int CommMan::commRank(int comm, int wrank)
{
    RankPair::iterator it;

    if(comm == MPI_COMM_SELF)return 0;

    if(comm == MPI_COMM_WORLD)return wrank;

    if(!exist(comm))return -1;

    for(it = communicators_[comm]->begin(); it != communicators_[comm]->end(); it ++)
    {
        if(it->second == wrank)return it->first;
    }

    return -1;
}

int CommMan::commTrans(int comm, int grank, int comm2)
{
    if(comm == MPI_COMM_SELF)return -1;
        
    int wrank = grank;

    if(comm != MPI_COMM_WORLD)
    {
        if(!exist(comm))return -1;

        wrank = (*communicators_[comm])[grank];
    }

    return commRank(comm2, wrank);
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