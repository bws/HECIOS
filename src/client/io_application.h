#ifndef IO_APPLICATION_H
#define IO_APPLICATION_H

#include <omnetpp.h>
#include "client_fs_state.h"
class FSOpenFile;
class IOTrace;
class IOTraceRecord;

/**
 * Model of an application process.
 */
class IOApplication : public cSimpleModule
{
public:
    /** Constructor */
    IOApplication() : cSimpleModule(), trace_(0), rank_(-1) {};
    
    /** @return the file descriptor for a file id */
    FSOpenFile* getDescriptor(int fileId) const;

    /** @return the MPI application's canonical process rank */
    int getRank() const {return rank_;};
    
protected:
    /** Implementation of initialize */
    virtual void initialize();

    /** Implementation of finish */
    virtual void finish();

    /** Implementation of handleMessage */
    virtual void handleMessage(cMessage* msg);

    /** Get the next message to send */
    virtual cMessage* getNextMessage();

    /** Create a cMessage from an IOTraceRecord */
    virtual cMessage* createMessage(IOTraceRecord* rec);
    
    /** */
    void setDescriptor(int fileId, FSOpenFile* descriptor);
    
private:

    IOTrace* trace_;
    int rank_;
    ClientFSState clientState_;

    int inGate_;
    int outGate_;

    /** */
    std::map<int, FSOpenFile*> descriptorById_;

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
