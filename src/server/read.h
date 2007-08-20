#ifndef READ_H
#define READ_H

class cMessage;
class spfsReadRequest;
class FSServer;

/**
 * Perform server side read processing
 */
class Read
{
public:

    /** Constructor */
    Read(FSServer* module, spfsReadRequest* readReq);

    /**
     * Perform server side read processing
     */
    void handleServerMessage(cMessage* msg);

protected:

    /**
     * Create and send the disk requests
     */
    void enterReadData();
    
    /**
     * Create and send the final read response
     */
    void enterFinish();
    
private:

    /** The parent module */
    FSServer* module_;
    
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
