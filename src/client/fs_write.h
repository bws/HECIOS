#ifndef FS_WRITE_H
#define FS_WRITE_H
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
class spfsMPIFileWriteAtRequest;
class spfsWriteResponse;

/**
 * Class responsible for performing client-side file writes
 */
class FSWrite
{
public:

    /** Constructor */
    FSWrite(fsModule* module, spfsMPIFileWriteAtRequest* writeReq);
    
    /** Handle MPI-Open Message */
    void handleMessage(cMessage* msg);

protected:

    /**
     * @return The next state transition, see implementation for details
     */
    void exitInit(spfsMPIFileWriteAtRequest* writeReq);

    /** Send server write requests */
    void enterWrite();

    /** Count server write initiation responses */
    void exitCountResponses(bool& outHasReceivedAllResponses);
        
    /** Send the final response */
    void enterFinish();
    
private:

    /** The filesystem module */
    fsModule* fsModule_;

    /** The originating MPI write request */
    spfsMPIFileWriteAtRequest* writeReq_;
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
