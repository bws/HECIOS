#ifndef READ_H
#define READ_H

class cMessage;
class spfsReadRequest;

/**
 *
 */
class Read
{
public:

    /** Constructor */
    Read(spfsReadRequest* readReq);

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
    void exitFinish(spfsReadRequest* lookupReq);
    
private:

    /** The originating read request */
    spfsReadRequest* readReq_;
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
