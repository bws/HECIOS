#ifndef FS_COLLECTIVE_GET_ATTRIBUTES_SM_H
#define FS_COLLECTIVE_GET_ATTRIBUTES_SM_H
//
// This file is part of Hecios
//
// Copyright (C) 2007,2008,2009 Brad Settlemyer
//
// This file is distributed WITHOUT ANY WARRANTY. See the file 'License.txt'
// for details on this and other legal matters.
//
#include "filename.h"
#include "fs_state_machine.h"
#include "pfs_types.h"
class cFSM;
class cMessage;
class FSClient;
class spfsMPIRequest;

/**
 * Class responsible for getting the attributes for a handle collectively
 */
class FSCollectiveGetAttributesSM : public FSStateMachine
{
public:
    /** Constructor for get attribute state machine */
    FSCollectiveGetAttributesSM(const Filename& filename,
                                bool calculateSize,
                                spfsMPIRequest* mpiRequest,
                                FSClient* client);

protected:
    /** Message processing for client name lookup*/
    virtual bool updateState(cFSM& currentState, cMessage* msg);

private:
    /** @return true if the attributes are in the client cache */
    bool isAttrCached();

    /** Send the requests to get the data attributes */
    void getAttributesCollective();

    /** @return Cache the file's attributes */
    void cacheAttributes();

    /** Handle to retrieve the attributes */
    FSHandle handle_;

    /** Flag to indicate whether the file size needs to be calculated */
    bool calculateSize_;

    /** The originating MPI request */
    spfsMPIRequest* mpiReq_;

    /** The filesystem client module */
    FSClient* client_;
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
