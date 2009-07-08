#ifndef FS_GET_ATTRIBUTES_GENERIC_SM_H
#define FS_GET_ATTRIBUTES_GENERIC_SM_H
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

/**
 * Class responsible for getting the attributes for a handle
 */
template<class AppRequestType>
class FSGetAttributesGenericSM : public FSStateMachine
{
public:
    /** Constructor for get attribute state machine */
    FSGetAttributesGenericSM(const Filename& filename,
                             bool calculateSize,
                             AppRequestType* appRequest,
                             FSClient* client);

protected:
    /** Message processing for client name lookup*/
    virtual bool updateState(cFSM& currentState, cMessage* msg);

private:
    /** @return true if the lookup name is cached */
    bool isAttrCached();

    /** Send the request to file's metadata attributes */
    void getMetadata();

    /** Send the requests to get the data attributes */
    void getDataAttributes();

    /** @return true when all the data attributes responses have been recvd */
    bool countResponse();

    /** @return Cache the file's attributes */
    void cacheAttributes();

    /** Handle to retrieve the attributes */
    FSHandle handle_;

    /** Flag to indicate whether the file size needs to be calculated */
    bool calculateSize_;

    /** The originating application request */
    AppRequestType* appReq_;

    /** The filesystem client module */
    FSClient* client_;
};

// Convenience typedef
class spfsMPIRequest;
typedef FSGetAttributesGenericSM<spfsMPIRequest> FSGetAttributesSM;

// g++ does not support export yet, so just do this to hack around that
#include "fs_get_attributes_generic_sm.cc"
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
