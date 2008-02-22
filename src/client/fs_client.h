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

class FSClient : public cSimpleModule
{
public:
    /** Attributes contains the following fields:
     *  UID(4), GID(4), Perms(4), ATime(8), MTime(8), CTime(8), (4), (4),
     *  (16)
     */
    static const unsigned int OBJECT_ATTRIBUTES_SIZE = 60;

    /** Credentials contains the following fields:
     *  UID(4), GID(4)
     */
    static const unsigned int CREDENTIALS_SIZE = 8;

    /** Constructor */
    FSClient();
    
    /** @return a reference to the client filesystem state */
    ClientFSState& fsState() { return clientState_; };

    /** @return the application outbound gate id */
    int getAppOutGate() const { return appOutGateId_; };
    
    /** @return the network outbound gate id */
    int getNetOutGate() const { return netOutGateId_; };
    
protected:
    /** Initialize the module */
    virtual void initialize();

    /** Finalize the module */
    virtual void finish();

    /** Handle incoming messages */
    virtual void handleMessage(cMessage *msg);

private:

    /**
     * Process the incoming message based on the originating request type
     *
     * @param the originating request type
     * @param the message to process
     */
    void processMessage(cMessage* request, cMessage* msg);

    /** Collect statistics on server responses */
    void collectServerResponseData(cMessage* serverResponse);
    
    int appInGateId_;
    int appOutGateId_;
    int netInGateId_;
    int netOutGateId_;

    /** Client file system state */
    ClientFSState clientState_;

    /** Data collection */
    cOutVector createDirEntDelay;
    cOutVector createObjectDelay;
    cOutVector flowDelay;
    cOutVector getAttrDelay;
    cOutVector lookupPathDelay;
    cOutVector readDelay;
    cOutVector setAttrDelay;
    cOutVector writeCompleteDelay;
    cOutVector writeDelay;
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
