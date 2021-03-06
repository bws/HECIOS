#ifndef HARD_DISK_H
#define HARD_DISK_H
//
// This file is part of Hecios
//
// Copyright (C) 2007,2008,2009 Brad Settlemyer
//
// This file is distributed WITHOUT ANY WARRANTY. See the file 'License.txt'
// for details on this and other legal matters.
//
#include <stdint.h>
#include <omnetpp.h>
#include "basic_types.h"

/** @brief Abstract base class for hard disks  */
class HardDisk : public cSimpleModule
{
public:

    /** Constructor */
    HardDisk();

    /** Destructor */
    virtual ~HardDisk();

protected:
    /**
     *  This is the initialization routine for this simulation module.
     */
    virtual void initialize();

    /**
     *  This is the finalization routine for this simulation module.
     */
    virtual void finish();

    /**
     *  This is the finalization routine for this simulation module.
     */
    virtual void handleMessage(cMessage *msg);

    /** @return the disks basic block size in bytes */
    long getBasicBlockSize() const;

    /** Register the time spent in disk service for this request */
    void registerDiskDelay(double delay);

private:

    /** @return the disk service *completion* time for the message */
    virtual double service(LogicalBlockAddress blockNumber, bool isRead) = 0;

    /** @return the basic block size for the disk model */
    virtual uint32_t basicBlockSize() const = 0;

    /** Out gate id */
    int outGateId_;

    /** Total number of blocks read */
    double totalBlocksRead_;

    /** Total number of blocks written */
    double totalBlocksWritten_;

    /** Total time spent accessing the disk */
    double totalDelay_;
};

/**
 * BasicDiskModel capable of simulating most of the disk model described
 * in the 1994 IEEE Computer article: "An Introduction to Disk Drive Modeling"
 * by Ruemmler and Wilkes.
 *
 * Model does account for controller delay, disk architecture, and density,
 * however it lacks adequate modelling of position modeling, detailed
 * disk layout, or possible read/write parity based optimizations
 *
 */
class BasicModelDisk : public HardDisk
{
public:

    /** Initialize Omnet model */
    virtual void initialize();

protected:

    virtual void handleMessage(cMessage* msg);

private:

    /** Concrete implementation of service method */
    virtual double service(LogicalBlockAddress blockNumber, bool isRead);

    /** @return the basic block size for the disk model */
    virtual uint32_t basicBlockSize() const;

    // Data descibing disk characteristics
    double fixedControllerReadOverheadSecs_;
    double fixedControllerWriteOverheadSecs_;
    double trackSwitchTimeSecs_;
    double averageReadSeekSecs_;
    double averageWriteSeekSecs_;

    uint64_t capacity_;
    uint32_t numCylinders_;
    uint32_t numHeads_;
    uint32_t numSectors_;
    uint32_t sectorsPerTrack_;
    uint32_t rpms_;

    // Data derived from disk characteristics
    uint32_t headsPerCylinder_;
    double timePerRevolution_;
    double timePerSector_;

    // Disk state
    uint32_t lastCylinder_;
    uint32_t lastHead_;
    simtime_t lastCompletionTime_;
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


