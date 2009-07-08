#ifndef SET_ATTR_H
#define SET_ATTR_H
//
// This file is part of Hecios
//
// Copyright (C) 2007,2008,2009 Brad Settlemyer
//
// This file is distributed WITHOUT ANY WARRANTY. See the file 'License.txt'
// for details on this and other legal matters.
//
class cMessage;
class spfsSetAttrRequest;
class FSServer;

/**
 * Provides the FSM for procesing Set Attribute requests to this server
 */
class SetAttr
{
public:
    /** Constructor */
    SetAttr(FSServer* module, spfsSetAttrRequest* setAttrReq);

    /**
     * Handle the arrival of a message during set attr processing
     */
    void handleServerMessage(cMessage* msg);

protected:
    /**
     * Perform action when entering the WRITE_ATTR state
     */
    void enterWriteAttr();

    /**
     * Perform action when entering the FINISH state
     */
    void enterFinish();

private:
    /** The parent module */
    FSServer* module_;

    /** The originating set attr request */
    spfsSetAttrRequest* setAttrReq_;
};

#endif

/*
 * Local variables:
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 *
 * vim: ts=4 sts=4 sw=4 expandtab
 */
