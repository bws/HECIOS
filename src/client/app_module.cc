#include "umd_io_trace.h"
#include <omnetpp.h>

/**
 * Model of an application process.
 */
class AppModule : public cSimpleModule
{
public:
    /** Constructor */
    AppModule() : cSimpleModule(), trace_(0) {};
    
protected:
    /** Implementation of initialize */
    virtual void initialize();

    /** Implementation of finish */
    virtual void finish();

    /** Implementation of handleMessage */
    virtual void handleMessage(cMessage* msg);

private:
    IOTrace* trace_;
};

// OMNet Registriation Method
Define_Module(AppModule);

/**
 * Construct an I/O trace using configuration supplied tracefile(s)
 */
void AppModule::initialize()
{
    // Construct a trace and begin sending events
    trace_ = new UMDIOTrace(8, "cholesky.trace");
}

/**
 * Cleanup and tally statistics
 */
void AppModule::finish()
{
    delete trace_;
    trace_ = 0;
}

/**
 * Handle MPI-IO Response messages
 */
void AppModule::handleMessage(cMessage* msg)
{
}

/*
 * Local variables:
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 *
 * vim: ts=8 sts=4 sw=4 expandtab
 */
