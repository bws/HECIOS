#ifndef GET_ATTR_H
#define GET_ATTR_H

class cMessage;
class spfsGetAttrRequest;
class FSServer;

/**
 *
 */
class GetAttr
{
public:

    /** Constructor */
    GetAttr(FSServer* module, spfsGetAttrRequest* getAttrReq);

    /**
     *
     */
    void handleServerMessage(cMessage* msg);

protected:

    /**
     *
     */
    void enterFinish();
    
    /**
     *
     */
    void exitFinish(spfsGetAttrRequest* lookupReq);
    
private:

    /** The parent module */
    FSServer* module_;
    
    /** The originating get attr request */
    spfsGetAttrRequest* getAttrReq_;
};

#endif

/*
 * Local variables:
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 *
 * vim: ts=4 sts=4 sw=4 expandtab foldmethod=marker
 */
