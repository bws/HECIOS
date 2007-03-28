#ifndef FS_SERVER_H
#define FS_SERVER_H

#include <string>
#include <omnetpp.h>
#include "pfs_types.h"

/**
 * Model of a file system client library.
 */
class FSServer : public cSimpleModule
{
public:
    /** Constructor */
    FSServer() : cSimpleModule() {};

    std::string getName() const { return serverName_; };
    
    HandleRange getHandleRange() const { return range_; };
    
protected:
    
    /** Implementation of initialize */
    virtual void initialize();

    /** Implementation of finish */
    virtual void finish() {};

    /** Implementation of handleMessage */
    virtual void handleMessage(cMessage* msg);

private:

    /** */
    static int serverNumber_;

    /** */
    std::string serverName_;

    /** */
    HandleRange range_;
};

#endif

/*
 * Local variables:
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 *
 * vim: ts=8 sts=4 sw=4 expandtab
 */
