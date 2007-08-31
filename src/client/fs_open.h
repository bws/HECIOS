#ifndef FS_OPEN_H
#define FS_OPEN_H
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

class cFSM;
class cMessage;
class fsModule;
class spfsMPIFileOpenRequest;
class spfsCreateResponse;
class spfsCreateDirEntResponse;
class spfsGetAttrResponse;
class spfsLookupPathResponse;
class spfsSetAttrResponse;

/**
 * Class responsible for opening a file
 */
class FSOpen
{
public:

    /** Constructor */
    FSOpen(fsModule* module, spfsMPIFileOpenRequest* openReq);
    
    /** Handle MPI-Open Message */
    void handleMessage(cMessage* msg);

protected:

    /**
     * @return The next state transition, see implementation for details
     */
    void exitInit(spfsMPIFileOpenRequest* openReq,
                  bool& outIsInDirCache,
                  bool& outIsInAttrCache);

    /** Lookup */
    void enterLookup();

    void exitLookup(spfsLookupPathResponse* lookupResponse,
                    bool& outIsCreate, bool& outIsFullLookup,
                    bool& outIsMissingAttr, bool& outIsPartialLookup);

    /** Create meta objects */
    void enterCreateMeta();

    void exitCreateMeta(spfsCreateResponse* createResp);

    /** Create data objects */
    void enterCreateData();

    void exitCreateData(spfsCreateResponse* createResp,
                        bool& outIsDoneCreatingDataObjects);

    /** Set attributes */
    void enterWriteAttr();

    void exitWriteAttr(spfsSetAttrResponse* sattrResp);

    /** Write directory entry */
    void enterWriteDirEnt();

    void exitWriteDirEnt(spfsCreateDirEntResponse* wdirentResp);

    /** Read attributes */
    void enterReadAttr();

    void exitReadAttr(spfsGetAttrResponse* gattrResp);

    /** Finalize operation */
    void enterFinish();
    
    void errorNF(spfsMPIFileOpenRequest* openReq);

    void errorExcl(spfsMPIFileOpenRequest* openReq);
    
private:

    /** The filesystem module */
    fsModule* fsModule_;

    /** The originating MPI Open request */
    spfsMPIFileOpenRequest* openReq_;
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
