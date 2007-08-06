#ifndef HARD_DISK_H
#define HARD_DISK_H

/**
 * @file hard_disk.h
 * @brief Disk Simulation Modules
 */

#include <omnetpp.h>

/** @brief Abstract base class for hard disks  */
class HardDisk : public cSimpleModule
{
  public:

    /** Constructor */
    HardDisk();

    /** Destructor */
    virtual ~HardDisk();

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

private:
    
    /** @return the disk service time for the message */
    virtual double service(long long blockNumber, bool isRead) = 0;

    /** @return the basic block size for the disk model */
    virtual long basicBlockSize() const = 0;
    
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
    virtual double service(long long blockNumber, bool isRead);

    /** @return the basic block size for the disk model */
    virtual long basicBlockSize() const;

    // Data descibing disk characteristics
    double fixedControllerReadOverheadSecs_;
    double fixedControllerWriteOverheadSecs_;
    double trackSwitchTimeSecs_;
    double averageReadSeekSecs_;
    double averageWriteSeekSecs_;
    
    long long capacity_;
    long numCylinders_;
    long tracksPerCylinder_;
    long sectorsPerTrack_;
    long rpms_;

    // Data derived from disk characteristics
    long sectorsPerCylinder_;
    long numSectors_;
    double timePerRevolution_;
    double timePerSector_;

    // Disk state
    long lastCylinder_;
    double lastTime_;
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


