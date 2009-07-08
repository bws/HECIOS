//
// This file is part of Hecios
//
// Copyright (C) 2007,2008,2009 Brad Settlemyer
//
// This file is distributed WITHOUT ANY WARRANTY. See the file 'License.txt'
// for details on this and other legal matters.
//
#include "fs_client_operation.h"
#include "file_builder.h"
#include "fs_operation_state.h"
#include "mpi_proto_m.h"

bool FSClientOperation::fileExists(const Filename& filename)
{
    return (0 != FileBuilder::instance().getMetaData(filename));
}

FSClientOperation::FSClientOperation(spfsMPIRequest* mpiRequest)
    : FSOperation(mpiRequest->getOpState()),
      mpiRequest_(mpiRequest)
{
}

FSClientOperation::~FSClientOperation()
{
    // Store the updated operation state back into the mpi message
    mpiRequest_->setOpState(FSOperation::state());
}

/*
 * Local variables:
 *  indent-tabs-mode: nil
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 *
 * vim: ts=8 sts=4 sw=4 expandtab
 */
