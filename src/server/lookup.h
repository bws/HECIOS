#ifndef LOOKUP_H
#define LOOKUP_H

class cMessage;
class spfsLookupPathRequest;
class FSServer;

/**
 *
 */
class Lookup
{
public:

    /** Constructor */
    Lookup(FSServer* module, spfsLookupPathRequest* lookupReq);

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
    void exitFinish(spfsLookupPathRequest* lookupReq);
    
private:

    /** The parent module */
    FSServer* module_;
    
    /** The originating create request */
    spfsLookupPathRequest* lookupReq_;
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
