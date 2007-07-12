#ifndef FS_WRITE_H
#define FS_WRITE_H

class cMessage;
class fsModule;
class spfsMPIFileWriteAtRequest;
class spfsWriteResponse;

/**
 * Class responsible for performing client-side file writes
 */
class FSWrite
{
public:

    /** Constructor */
    FSWrite(fsModule* module, spfsMPIFileWriteAtRequest* writeReq);
    
    /** Handle MPI-Open Message */
    void handleMessage(cMessage* msg);

protected:

    /**
     * @return The next state transition, see implementation for details
     */
    void exitInit(spfsMPIFileWriteAtRequest* writeReq);

    /** Send server write requests */
    void enterWrite();

    /** Count server write initiation responses */
    void exitCountResponses(bool& outHasReceivedAllResponses);
        
    /** Send the final response */
    void enterFinish();
    
private:

    /** The filesystem module */
    fsModule* fsModule_;

    /** The originating MPI write request */
    spfsMPIFileWriteAtRequest* writeReq_;
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
