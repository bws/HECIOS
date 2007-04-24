#ifndef FS_OPEN_H
#define FS_OPEN_H

#include <omnetpp.h>
class cFSM;
class fsModule;
class spfsMPIFileOpenRequest;

class FSOpen
{
public:

    /** Handle Open Message */
    void handleFSRequest(spfsMPIFileOpenRequest* openReq,
                         fsModule* module);

protected:

    /**
     * @return The next state transition, see implementation for details
     */
    void exitInit(spfsMPIFileOpenRequest* openReq,
                  fsModule* module,
                  bool& outIsInDirCache,
                  bool& outIsInAttrCache);

    /** Lookup */
    void enterLookup(spfsMPIFileOpenRequest* openReq, fsModule* module);

    void exitLookup(spfsMPIFileOpenRequest* openReq);

    /** Create meta objects */
    void enterCreateMeta(spfsMPIFileOpenRequest* openReq);

    void exitCreateMeta(spfsMPIFileOpenRequest* openReq);

    /** Create data objects */
    void enterCreateData(spfsMPIFileOpenRequest* openReq);

    void exitCreateData(spfsMPIFileOpenRequest* openReq);

    /** Set attributes */
    void enterWriteAttr(spfsMPIFileOpenRequest* openReq);

    void exitWriteAttr(spfsMPIFileOpenRequest* openReq);

    /** Write directory entry */
    void enterWriteDirEnt(spfsMPIFileOpenRequest* openReq);

    void exitWriteDirEnt(spfsMPIFileOpenRequest* openReq);

    /** Read attributes */
    void enterReadAttr(spfsMPIFileOpenRequest* openReq);

    void exitReadAttr(spfsMPIFileOpenRequest* openReq);

    /** Finalize operation */
    void enterFinish(spfsMPIFileOpenRequest* openReq);
    
    void errorNF(spfsMPIFileOpenRequest* openReq);

    void errorExcl(spfsMPIFileOpenRequest* openReq);
    
private:
    cFSM currentState_;
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
