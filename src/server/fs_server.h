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

    void setNumber(size_t number);
    
protected:
    
    /** Implementation of initialize */
    virtual void initialize();

    /** Implementation of finish */
    virtual void finish() {};

    /** Implementation of handleMessage */
    virtual void handleMessage(cMessage* msg);

private:

    /** Unique server number */
    size_t serverNumber_;

    /** Unique server name */
    std::string serverName_;

    /** The handle range for this server */
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
