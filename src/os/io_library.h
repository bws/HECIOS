#ifndef IO_LIBRARY_H
#define IO_LIBRARY_H
//
// This file is part of Hecios
//
// Copyright (C) 2006 Joel Sherrill <joel@oarcorp.com>
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
class NativeFileSystem;

/**
 * Abstract base class for I/O library models
 */
class IOLibrary : public cSimpleModule
{
public:

protected:

    /**
     *  This is the initialization routine for this simulation module.
     */
    virtual void initialize();

    /**
     *  This is the finalization routine for this simulation module.
     */
    virtual void handleMessage(cMessage *msg);

private:

    /** in gate connection id */
    int inGateId_;

    /** out gate connection id */
    int outGateId_;

    /** request gate connection id */
    int requestGateId_;
};

/**
 *
 */
class ListIOLibrary : public IOLibrary
{
};

/**
 * I/O library model that passes requests through with no translation
 */
class PassThroughIOLibrary : public IOLibrary
{
};

#endif

/*
 * Local variables:
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 *
 * vim: ts=8 sts=4 sw=4 expandtab
 */
