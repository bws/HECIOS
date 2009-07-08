#ifndef FS_CLIENT_OPERATION_H
#define FS_CLIENT_OPERATION_H
//
// This file is part of Hecios
//
// Copyright (C) 2007,2008,2009 Brad Settlemyer
//
// This file is distributed WITHOUT ANY WARRANTY. See the file 'License.txt'
// for details on this and other legal matters.
//
#include "fs_operation.h"
class Filename;
class spfsMPIRequest;

/**
 * A client-side file system operation.  This class exists to ensure the
 * operation state is updated correctly in the MPI request message.
 */
class FSClientOperation : public FSOperation
{
public:
    /** @return true if the file exists in the file system */
    static bool fileExists(const Filename& filename);

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
