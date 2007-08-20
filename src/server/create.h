#ifndef CREATE_H
#define CREATE_H

class cMessage;
class spfsCreateRequest;
class FSServer;

/**
 *
 */
class Create
{
public:

    /** Constructor */
    Create(FSServer* module, spfsCreateRequest* createReq);

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
    void exitFinish(spfsCreateRequest* createReq);
    
private:

    /** The parent module */
    FSServer* module_;
    
    /** The originating create request */
    spfsCreateRequest* createReq_;
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
