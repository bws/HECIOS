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
typedef std::map<int, int> RankPair;

#define MPI_COMM_WORLD CommMan::instance().commWorld()
#define MPI_COMM_SELF CommMan::instance().commSelf()

class CommMan : public Singleton<CommMan>
{
public:
    /** Enable singleton construction */
    friend class Singleton<CommMan>;
    
    //static CommMan * getInstance();

    int joinComm(int comm, int wrank);
    
    int commSize(int comm);
    int commRank(int comm, int wrank);
    int commTrans(int comm, int grank, int comm2);

    int commWorld();
    void commWorld(int world);

    int commSelf();
    void commSelf(int self);

protected:
    std::map<int, RankPair *> communicators_;
    bool exist(int comm);
    bool exist(int comm, int rank);
    CommMan(){commWorld_ = 1; commSelf_ = 0;};
    //static CommMan * commman_;

private:
    int commWorld_;
    int commSelf_;
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
