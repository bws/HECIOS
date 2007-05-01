#ifndef LOOKUP_H
#define LOOKUP_H

class cMessage;
class spfsLookupPathRequest;

/**
 *
 */
class Lookup
{
public:

    /** Constructor */
    Lookup(spfsLookupPathRequest* lookupReq);

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
    void exitFinish(spfsLookupPathRequest* lookupReq);
    
private:

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
