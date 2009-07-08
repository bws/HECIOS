#ifndef ACCESS_MANAGER_H
#define ACCESS_MANAGER_H
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

/**
 * Abstract base class for single-server queues
 */
class AccessManager : public cSimpleModule
{
public:
    /**
     *  This is the constructor for this simulation module.
     */
    AccessManager();

    /**
     *  This is the destructor for this simulation module.
     */
    ~AccessManager();

    /**
     *  This is the initialization routine for this simulation module.
     */
    virtual void initialize();

    /**
     *  This is the finalization routine for this simulation module.
     */
    virtual void finish();

    /**
     *  This is the message handling routine for this simulation module.
     *
     *  @param msg (in) is the new message to process
     */
    virtual void handleMessage(cMessage *msg);

protected:
    int fromSchedulerId;
    int fromDiskId;

    int getNextRequestGateId_;

    int nextRequestGateId_;

    int getBlockGateId_;

    int blockGateId_;
};

/**
 * AccessManager that passes through all requests
 */
class SimpleAccessManager : public AccessManager
{
  public:
    /**
     *  This is the constructor for this simulation module.
     */
    SimpleAccessManager();
};

/**
 * AccessManager that allows only one outstanding request at a time
 */
class MutexAccessManager : public AccessManager
{
  public:
    /**
     *  This is the constructor for this simulation module.
     */
    MutexAccessManager();

protected:

    virtual void initialize();

    virtual void finish();

    virtual void handleMessage( cMessage *msg );

private:

    bool hasIdleDisk_;
    cQueue pendingMessageQueue_;
};

#endif
