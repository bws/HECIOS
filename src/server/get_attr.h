#ifndef GET_ATTR_H
#define GET_ATTR_H
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
class spfsGetAttrRequest;
class FSServer;

/**
 *
 */
class GetAttr
{
public:

    /** Constructor */
    GetAttr(FSServer* module, spfsGetAttrRequest* getAttrReq);

    /**
     *
     */
    void handleServerMessage(cMessage* msg);

protected:

    /**
     *
     */
    void enterFinish();
    
    /**
     *
     */
    void exitFinish(spfsGetAttrRequest* lookupReq);
    
private:

    /** The parent module */
    FSServer* module_;
    
    /** The originating get attr request */
    spfsGetAttrRequest* getAttrReq_;
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
