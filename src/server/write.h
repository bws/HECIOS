#ifndef WRITE_H
#define WRITE_H

class cMessage;
class spfsWriteRequest;

/**
 *
 */
class Write
{
public:

    /** Constructor */
    Write(spfsWriteRequest* writeReq);

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
    void exitFinish(spfsWriteRequest* writeReq);
    
private:

    /** The originating write request */
    spfsWriteRequest* writeReq_;
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
