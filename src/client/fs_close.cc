#include "fs_close.h"
#include <iostream>
#include <omnetpp.h>
#include "fs_module.h"
#include "mpiio_proto_m.h"
#include "pfs_utils.h"
#include "pvfs_proto_m.h"
using namespace std;

FSClose::FSClose(fsModule* module, spfsMPIFileCloseRequest* closeReq)
    : fsModule_(module),
      closeReq_(closeReq)
{
    assert(0 != fsModule_);
    assert(0 != closeReq_);
}

// Processing that occurs upon receipt of an MPI-IO Open request
void FSClose::handleMessage(cMessage* msg)
{
    spfsMPIFileCloseResponse* mpiResp =
            new spfsMPIFileCloseResponse(0, SPFS_MPI_FILE_CLOSE_RESPONSE);
    mpiResp->setContextPointer(closeReq_);
    fsModule_->send(mpiResp, fsModule_->fsMpiOut);                 

}

/*
 * Local variables:
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 *
 * vim: ts=4 sts=4 sw=4 expandtab foldmethod=marker
 */
