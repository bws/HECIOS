#ifndef READ_H
#define READ_H
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
class spfsReadRequest;
class FSServer;

/**
 * Perform server side read processing
 */
class Read
{
public:

    /** Constructor */
    Read(FSServer* module, spfsReadRequest* readReq);

    /** Destructor */
    ~Read();

    /**
     * Perform server side read processing
     */
    void handleServerMessage(cMessage* msg);

protected:

    /** @return true if the file contains the offset/extent to be read */
    bool hasReadData();

    /**
     * Create and send the data flow requests
     */
    void startDataFlow();

    /**
     * Create and send the final read response
     */
    void sendFinalResponse();

    /**
     * No-op for now.
     */
    void finish();

private:

    /** The parent module */
    FSServer* module_;

    /** The originating read request */
    spfsReadRequest* readReq_;

    /** Flag indicating the orginating request should be deleted on exit */
    bool cleanupRequest_;
};

#endif

/*
 * Local variables:
 *  indent-tabs-mode: nil
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 *
 * vim: ts=4 sts=4 sw=4 expandtab foldmethod=marker
 */
