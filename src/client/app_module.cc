
#include <omnetpp.h>

class AppModule : public cSimpleModule
{
protected:
    virtual void initialize();

    virtual void handleMessage();
};

/*
 * Local variables:
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 *
 * vim: ts=8 sts=4 sw=4 expandtab
 */
