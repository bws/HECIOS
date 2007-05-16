#ifndef FS_CLOSE_H
#define FS_CLOSE_H

class cFSM;
class cMessage;
class fsModule;
class spfsMPIFileCloseRequest;

/**
 * Class responsible for performing client-side file close
 */
class FSClose
{
public:

    /** Constructor */
    FSClose(fsModule* module, spfsMPIFileCloseRequest* closeReq);
    
    /** Handle MPI-Open Message */
    void handleMessage(cMessage* msg);

private:

    /** The filesystem module */
    fsModule* fsModule_;

    /** The originating MPI close request */
    spfsMPIFileCloseRequest* closeReq_;
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
