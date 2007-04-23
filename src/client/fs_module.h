#ifndef FS_MODULE_H
#define FS_MODULE_H

#include <omnetpp.h>

class fsModule : public cSimpleModule
{
protected:
    virtual void initialize();
    virtual void finish();
    virtual void handleMessage(cMessage *msg);

public:
    int fsMpiOut;
    int fsMpiIn;
    int fsNetOut;
    int fsNetIn;
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
