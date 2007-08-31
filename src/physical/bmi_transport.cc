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
#include <iostream>
#include "pvfs_proto_m.h"
#include "umd_io_trace.h"
#include <omnetpp.h>
using namespace std;

/**
 * Model of a file system client library.
 */
class BMITransport : public cSimpleModule
{
public:
    /** Constructor */
    BMITransport() : cSimpleModule() {};
    
protected:
    /** Implementation of initialize */
    virtual void initialize() {};

    /** Implementation of finish */
    virtual void finish() {};

    /** Implementation of handleMessage */
    virtual void handleMessage(cMessage* msg);

};

// OMNet Registriation Method
Define_Module(BMITransport);

/**
 * Handle MPI-IO Response messages
 */
void BMITransport::handleMessage(cMessage* msg)
{
    switch(msg->kind())
    {
        case FS_CREATE_REQUEST:
            cerr << "BMITransport::handleMessage not yet implemented for kind: "
                 << msg->kind() << endl;
            break;
        case FS_REMOVE_REQUEST:
            cerr << "BMITransport::handleMessage not yet implemented for kind: "
                 << msg->kind() << endl;
            break;
        case FS_READ_REQUEST:
            cerr << "BMITransport::handleMessage not yet implemented for kind: "
                 << msg->kind() << endl;
            break;
        case FS_WRITE_REQUEST:
            cerr << "BMITransport::handleMessage not yet implemented for kind: "
                 << msg->kind() << endl;
           break;
        case FS_GET_ATTR_REQUEST:
            cerr << "BMITransport::handleMessage not yet implemented for kind: "
                 << msg->kind() << endl;
            break;
        case FS_SET_ATTR_REQUEST:
            cerr << "BMITransport::handleMessage not yet implemented for kind: "
                 << msg->kind() << endl;
            break;
        case FS_LOOKUP_PATH_REQUEST:
            cerr << "BMITransport::handleMessage not yet implemented for kind: "
                 << msg->kind() << endl;
            break;
        case FS_CREATE_DIR_ENT_REQUEST:
            cerr << "BMITransport::handleMessage not yet implemented for kind: "
                 << msg->kind() << endl;
            break;
        case FS_REMOVE_DIR_ENT_REQUEST:
            cerr << "BMITransport::handleMessage not yet implemented for kind: "
                 << msg->kind() << endl;
            break;
        case FS_CHANGE_DIR_ENT_REQUEST:
            cerr << "BMITransport::handleMessage not yet implemented for kind: "
                 << msg->kind() << endl;
            break;
        case FS_TRUNCATE_REQUEST:
            cerr << "BMITransport::handleMessage not yet implemented for kind: "
                 << msg->kind() << endl;
            break;
        case FS_MAKE_DIR_REQUEST:
            cerr << "BMITransport::handleMessage not yet implemented for kind: "
                 << msg->kind() << endl;
            break;
        default:
            cerr << "BMITransport::handleMessage not yet implemented for kind: "
                 << msg->kind() << endl;
            break;
    }
}

/*
 * Local variables:
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 *
 * vim: ts=8 sts=4 sw=4 expandtab
 */
