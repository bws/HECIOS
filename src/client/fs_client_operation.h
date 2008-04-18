#ifndef FS_CLIENT_OPERATION_H
#define FS_CLIENT_OPERATION_H
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
#include "fs_operation.h"
class spfsMPIRequest;

/**
 * A client-side file system operation.  This class exists to ensure the
 * operation state is updated correctly in the MPI request message.
 */
class FSClientOperation : public FSOperation
{
public:
    /** Constructor */
    FSClientOperation(spfsMPIRequest* mpiRequest);

    /** Destructor */
    virtual ~FSClientOperation();

private:
    /** Copy constructor hidden */
    FSClientOperation(const FSClientOperation& other);

    /** Assignment operator hidden */
    FSClientOperation& operator=(const FSClientOperation& other);

    /** The operation's originating MPI request */
    spfsMPIRequest* mpiRequest_;
};

#endif

/*
 * Local variables:
 *  indent-tabs-mode: nil
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 *
 * vim: ts=8 sts=4 sw=4 expandtab
 */
