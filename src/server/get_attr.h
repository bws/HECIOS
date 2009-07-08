#ifndef GET_ATTR_H
#define GET_ATTR_H
//
// This file is part of Hecios
//
// Copyright (C) 2007,2008,2009 Brad Settlemyer
//
// This file is distributed WITHOUT ANY WARRANTY. See the file 'License.txt'
// for details on this and other legal matters.
//
#include <omnetpp.h>
class spfsGetAttrRequest;
class FSServer;

/**
 * Provides the FSM for procesing Get Attribute requests to this server
 */
class GetAttr
{
public:
    /** Constructor */
    GetAttr(FSServer* module, spfsGetAttrRequest* getAttrReq);

    /**
     * Handle the arrival of a message during get attr processing
     */
    void handleServerMessage(cMessage* msg);

protected:

    /**
     * Perform action when entering the READ_ATTR state
     */
    void enterReadAttr();

    /**
     * Perform action when entering the FINISH state
     */
    void enterFinish();

private:
    /** The parent module */
    FSServer* module_;

    /** The originating get attr request */
    spfsGetAttrRequest* getAttrReq_;
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
