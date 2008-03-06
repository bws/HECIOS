#ifndef CREATE_DIR_ENT_H
#define CREATE_DIR_ENT_H
//
// This file is part of Hecios
//
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

class cMessage;
class spfsCreateDirEntRequest;
class FSServer;

/**
 * State machine for performing directory entry creation processing
 */
class CreateDirEnt
{
public:
    /** Constructor */
    CreateDirEnt(FSServer* module, spfsCreateDirEntRequest* createDirEntReq);

    /**
     * Handle message as part of the creation process
     */
    void handleServerMessage(cMessage* msg);

protected:

    /**
     * Send the file creation message to the OS
     */
    void writeDirEnt();
    
    /**
     * Send the final response to the client
     */
    void finish();
    
private:

    /** The parent module */
    FSServer* module_;
    
    /** The originating create directory entry request */
    spfsCreateDirEntRequest* createDirEntReq_;
};

#endif

/*
 * Local variables:
 *  indent-tabs-mode: nil
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 *
 * vim: ts=4 sts=4 sw=4 expandtab
 */
