#ifndef FS_MODULE_H
#define FS_MODULE_H
//
// This file is part of Hecios
//
// Copyright (C) 2007 Walt Ligon
// Copyright (C) 2007 Brad Settlemyer
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

#include <omnetpp.h>
#include "client_fs_state.h"

class fsModule : public cSimpleModule
{
public:
    int fsMpiOut;
    int fsMpiIn;
    int fsNetOut;
    int fsNetIn;

    /** @return a reference to the client filesystem state */
    ClientFSState& fsState() { return clientState_; };
    
protected:
    virtual void initialize();
    virtual void finish();
    virtual void handleMessage(cMessage *msg);

private:
    
    ClientFSState clientState_;

};

#endif

/*
 * Local variables:
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 *
 * vim: ts=4 sts=4 sw=4 expandtab foldmethod=marker
 */
