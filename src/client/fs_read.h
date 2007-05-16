#ifndef FS_READ_H
#define FS_READ_H

class cMessage;
class fsModule;
class spfsMPIFileReadRequest;
class spfsReadResponse;

/**
 * Class responsible for performing client-side file reads
 */
class FSRead
{
public:

    /** Constructor */
    FSRead(fsModule* module, spfsMPIFileReadRequest* readReq);
    
    /** Handle MPI Read Message */
    void handleMessage(cMessage* msg);

protected:

    /**  */
    void enterRead();

    /** */
    void enterFinish(spfsReadResponse* readResponse);
    
private:

    /** The filesystem module */
    fsModule* fsModule_;

    /** The originating MPI read request */
    spfsMPIFileReadRequest* readReq_;
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
