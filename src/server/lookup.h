#ifndef LOOKUP_H
#define LOOKUP_H
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
#include "pfs_types.h"
class cMessage;
class spfsLookupPathRequest;
class FSServer;

/**
 * Perform server name lookups
 */
class Lookup
{
public:
    /** Constructor */
    Lookup(FSServer* module, spfsLookupPathRequest* lookupReq);

    /** Handle server lookup request */
    void handleServerMessage(cMessage* msg);

protected:

    /** Lookup the handle in the directory entries on disk */
    void lookupName();

    /** @return true if the name is fully resolved */
    bool lookupIsComplete();

    /** @return true if the name does not exists */
    bool lookupIsFailed();

    /** @return true if no more local segments exist */
    bool localLookupIsComplete();
    
    /** Send the final response */
    void finish(FSLookupStatus lookupStatus);
    
private:
    /** The parent module */
    FSServer* module_;
    
    /** The originating create request */
    spfsLookupPathRequest* lookupReq_;
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
