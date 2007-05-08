#ifndef FS_MODULE_H
#define FS_MODULE_H

#include <omnetpp.h>
#include "client_fs_state.h"

class fsModule : public cSimpleModule
{
public:
    int fsMpiOut;
    int fsMpiIn;
    int fsNetOut;
    int fsNetIn;

    /** @return a reference to the client filesystem state */
    ClientFSState& fsState() { return clientState_; };
    
protected:
    virtual void initialize();
    virtual void finish();
    virtual void handleMessage(cMessage *msg);

private:
    
    ClientFSState clientState_;

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
