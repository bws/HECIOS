#ifndef CREATE_H
#define CREATE_H

class cMessage;
class spfsCreateRequest;

/**
 *
 */
class Create
{
public:

    /** Constructor */
    Create(spfsCreateRequest* createReq);

    /**
     *
     */
    cMessage* handleServerMessage(cMessage* msg);

protected:

    /**
     *
     */
    cMessage* enterFinish();
    
    /**
     *
     */
    void exitFinish(spfsCreateRequest* createReq);
    
private:

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
