#ifndef DISK_SCHEDULER_H
#define DISK_SCHEDULER_H
//
// This file is part of Hecios
//
// Copyright (C) 2006 Joel Sherrill <joel@oarcorp.com>
// Copyright (C) 2007,2008,2009 Brad Settlemyer
//
// This file is distributed WITHOUT ANY WARRANTY. See the file 'License.txt'
// for details on this and other legal matters.
//
#include <vector>
#include <omnetpp.h>
#include "basic_types.h"

/**
 * Scheduler entry for disk schedulers
 */
struct SchedulerEntry : public cObject
{
    LogicalBlockAddress lba;
    cMessage* request;
    bool isReadRequest;
};

/**
 * Abstract base class for Disk Schedulers
 */
class DiskScheduler : public cSimpleModule
{
  public:
    /**
     * Remove and return entries from the queue that match the address lba.
     *  If readsOnly is set to true, only identical reads are extracted, if
     *  set to false both reads and writes are extracted
     *
     * @param the queue to extract from
     * @param the block address to compare for equality
     * @param set to true to extract only reads, ow extract reads and writes
     * @return the extracted entries
     */
    static std::vector<SchedulerEntry*> extractIdenticalEntries(
        cQueue& queue, LogicalBlockAddress lba, bool readsOnly);

    /**
     *  This is the constructor for this simulation module.
     *
     *  @param namestr (in) is the name of the module
     *  @param parent (in) is the parent of this module
     *  @param stack (in) is the size in bytes of the stack for this module
     */
    DiskScheduler();

    /**
     *  This is the destructor for this simulation module.
     */
    ~DiskScheduler();

protected:

    virtual void initialize();

    virtual void finish();

    virtual void handleMessage(cMessage* msg);

    /** Perform initialization tasks on scheduler before messages arrive */
    virtual void initializeScheduler() = 0;

    /** @return true if the disk scheduler has no outstanding entries */
    virtual bool isEmpty() const = 0;

    /** Add entry to the scheduler */
    virtual void addEntry(SchedulerEntry* entry) = 0;

    /** @return the next scheduled entry */
    virtual SchedulerEntry* popNextEntry() = 0;

    /**
     * Remove a superceded request if one exists.  A read or write request
     * becomes superceded if an incoming write request is to the same address.
     *
     * @return the superceded request
     */
    virtual std::vector<SchedulerEntry*> popRequestsCompletedByRead(
        LogicalBlockAddress lba) = 0;

    /**
     * @return a list of all satisfied requests
     */
    virtual std::vector<SchedulerEntry*> popRequestsCompletedByWrite(
        LogicalBlockAddress lba) = 0;

private:

    int inGateId_;

    int outGateId_;

    int requestGateId_;
};

/**
 * First come, first served Disk Scheduler
 */
class FCFSDiskScheduler : public DiskScheduler
{
  public:
    /** Constructor */
    FCFSDiskScheduler();

protected:

    virtual void initializeScheduler() {};

    virtual bool isEmpty() const { return fcfsQueue.empty(); };

    virtual void addEntry( SchedulerEntry* entry);

    virtual SchedulerEntry* popNextEntry();

    virtual std::vector<SchedulerEntry*> popRequestsCompletedByRead(
        LogicalBlockAddress lba);

    virtual std::vector<SchedulerEntry*> popRequestsCompletedByWrite(
        LogicalBlockAddress lba);
private:

    cQueue fcfsQueue;

};

/**
 * Shortest Seek Time First Disk Scheduler
 */
class SSTFDiskScheduler : public DiskScheduler
{
public:
    /** Constructor */
    SSTFDiskScheduler();

protected:

    virtual void initializeScheduler();

    virtual bool isEmpty() const {return sstfQueue.empty(); };

    virtual void addEntry( SchedulerEntry* entry);

    virtual SchedulerEntry* popNextEntry();

    virtual std::vector<SchedulerEntry*> popRequestsCompletedByRead(
        LogicalBlockAddress lba);

    virtual std::vector<SchedulerEntry*> popRequestsCompletedByWrite(
        LogicalBlockAddress lba);
private:

    cQueue sstfQueue;

    LogicalBlockAddress currentAddress_;
};

/**
 * SCAN Disk Scheduler
 */
class ScanDiskScheduler : public DiskScheduler
{
public:
    /** Constructor */
    ScanDiskScheduler();

protected:

    virtual void initializeScheduler() {};

    virtual void addEntry( SchedulerEntry* entry);

    virtual SchedulerEntry* popNextEntry();

    virtual std::vector<SchedulerEntry*> popSupercededRequests(
        LogicalBlockAddress lba);

    virtual std::vector<SchedulerEntry*> popAllSatisfiedRequests(
        LogicalBlockAddress lba);
private:

    cQueue scanQueue;

    bool isGoingUp_;
};

/**
 * C-SCAN Disk Scheduler
 */
class CScanDiskScheduler : public DiskScheduler
{
public:
    /** Constructor */
    CScanDiskScheduler();

protected:

    virtual void initializeScheduler() {};

    virtual void addEntry( SchedulerEntry* entry);

    virtual SchedulerEntry* popNextEntry();

    virtual std::vector<SchedulerEntry*> popSupercededRequests(
        LogicalBlockAddress lba);

    virtual std::vector<SchedulerEntry*> popAllSatisfiedRequests(
        LogicalBlockAddress lba);
private:

    cQueue sstfQueue;

    LogicalBlockAddress currentBlock_;
};

/**
 * N-Step SCAN Disk Scheduler
 */
class NStepScanDiskScheduler : public DiskScheduler
{
public:
    /** Constructor */
    NStepScanDiskScheduler();

protected:

    virtual void initializeScheduler() {};

    virtual void addEntry( SchedulerEntry* entry);

    virtual SchedulerEntry* popNextEntry();

    virtual std::vector<SchedulerEntry*> popSupercededRequests(
        LogicalBlockAddress lba);

    virtual std::vector<SchedulerEntry*> popAllSatisfiedRequests(
        LogicalBlockAddress lba);
private:

    cQueue scanQueue;

    bool isGoingUp_;
};

/**
 * N-Step C-SCAN Disk Scheduler
 */
class NStepCScanDiskScheduler : public DiskScheduler
{
public:
    /** Constructor */
    NStepCScanDiskScheduler();

protected:

    virtual void initializeScheduler() {};

    virtual void addEntry( SchedulerEntry* entry);

    virtual SchedulerEntry* popNextEntry();

    virtual std::vector<SchedulerEntry*> popSupercededRequests(
        LogicalBlockAddress lba);

    virtual std::vector<SchedulerEntry*> popAllSatisfiedRequests(
        LogicalBlockAddress lba);
private:

    cQueue scanQueue;
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
