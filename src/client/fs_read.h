#ifndef FS_READ_H
#define FS_READ_H
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
class fsModule;
class spfsMPIFileReadAtRequest;
class spfsReadResponse;

/**
 * Class responsible for performing client-side file reads
 */
class FSRead
{
public:

    /** Constructor */
    FSRead(fsModule* module, spfsMPIFileReadAtRequest* readReq);

    /** Destructor */
    virtual ~FSRead() {};
    
    /** Handle MPI Read Message */
    void handleMessage(cMessage* msg);

protected:

    /**  Construct server read requests */
    virtual void enterRead();

    /** Accumulate server read responses */
    virtual void exitCountResponses(bool& outHasReceivedAllResponses);

    /** Send final client response */
    virtual void enterFinish();
    
private:

    /** The filesystem module */
    fsModule* fsModule_;

    /** The originating MPI read request */
    spfsMPIFileReadAtRequest* readReq_;
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
