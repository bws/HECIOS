#ifndef FS_WRITE_H
#define FS_WRITE_H

class cMessage;
class fsModule;
class spfsMPIFileWriteRequest;
class spfsWriteResponse;

/**
 * Class responsible for performing client-side file writes
 */
class FSWrite
{
public:

    /** Constructor */
    FSWrite(fsModule* module, spfsMPIFileWriteRequest* writeReq);
    
    /** Handle MPI-Open Message */
    void handleMessage(cMessage* msg);

protected:

    /**
     * @return The next state transition, see implementation for details
     */
    void exitInit(spfsMPIFileWriteRequest* writeReq);

    /**  */
    void enterWrite();

    /** */
    void enterFinish(spfsWriteResponse* writeResponse);
    
private:

    /** The filesystem module */
    fsModule* fsModule_;

    /** The originating MPI write request */
    spfsMPIFileWriteRequest* writeReq_;
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
