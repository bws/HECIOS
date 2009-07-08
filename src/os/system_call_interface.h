#ifndef SYSTEM_CALL_INTERFACE_H
#define SYSTEM_CALL_INTERFACE_H
//
// This file is part of Hecios
//
// Copyright (C) 2006 Joel Sherrill <joel@oarcorp.com>
// Copyright (C) 2007,2008,2009 Brad Settlemyer
//
// This file is distributed WITHOUT ANY WARRANTY. See the file 'License.txt'
// for details on this and other legal matters.
//
#include <omnetpp.h>
#include "os_proto_m.h"
#include "serial_message_scheduler.h"

/**
 * Abstract base class for system call interface models
 */
class SystemCallInterface : public cSimpleModule
{
public:

    /** Constructor */
    SystemCallInterface();

protected:

    /**
     *  This is the initialization routine for this simulation module.
     */
    virtual void initialize();

    /**
     *  This is the finalization routine for this simulation module.
     */
    virtual void handleMessage(cMessage *msg);

    int inGateId_;

    int outGateId_;

    int requestGateId_;
};

/**
 * simple System call model that passes requests through with no translation
 */
class PassThroughSystemCallInterface : public SystemCallInterface
{
  public:
    /**
     *  This is the constructor for this simulation module.
     */
    PassThroughSystemCallInterface();
};

/**
 * System call model that sequentializes requests and applies a system call
 * delay
 */
class SequentialSystemCallInterface : public SystemCallInterface
{
public:
    /**
     *  This is the constructor for this simulation module.
     */
    SequentialSystemCallInterface();

protected:
    /** This is the initialization routine for this simulation module. */
    virtual void initialize();

    /** This is the finalization routine for this simulation module. */
    virtual void handleMessage(cMessage *msg);

private:
    /** */
    SerialMessageScheduler messageInScheduler_;

    /** */
    SerialMessageScheduler messageOutScheduler_;

    /** System call overhead */
    double overheadSecs_;

    /** Additional read system call overhead */
    double addReadOverheadSecs_;

    /** Additional write system call overhead */
    double addWriteOverheadSecs_;

};

#endif

/*
 * Local variables:
 *  indent-tabs-mode: nil
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 *
 * vim: ts=8 sts=4 sw=4 expandtab
 */
