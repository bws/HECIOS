#ifndef WRITE_H
#define WRITE_H
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
class spfsWriteRequest;
class FSServer;

/**
 * State machine performing the server write process
 */
class Write
{
public:

    /** Constructor */
    Write(FSServer* module, spfsWriteRequest* writeReq);

    /**
     * Handle server message
     */
    void handleServerMessage(cMessage* msg);

protected:

    /** Start the server side data flow processing for this write */
    void startDataFlow();

    /** Send the response indicating flow processing is ready */
    void sendResponse();

    /** Send response indicating the data has been commited to storage */
    void sendCompletionResponse();
    
private:

    /** The parent module */
    FSServer* module_;
    
    /** The originating write request */
    spfsWriteRequest* writeReq_;
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
