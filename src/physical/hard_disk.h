#ifndef HARD_DISK_H
#define HARD_DISK_H
/**
 * @file hard_disk.h
 * @brief Disk Simulation Modules
 *
 * Note: These classes are adapted from the FSS simulator project and are
 * licensed only under the GPL.  The FSS project is available at:
 * http://www.omnetpp.org/filemgmt/singlefile.php?lid=104 and
 * http://www.omnetpp.org/doc/FSS-doc/neddoc/index.html
 */

/*
 *  $Id: hard_disk.h,v 1.3 2007/08/03 15:05:21 bradles Exp $
 */

#include <omnetpp.h>

/**
 * @brief Abstract base class for disks
 */
class AbstractDisk : public cSimpleModule
{
  public:
    /**
     *  This is the constructor for this simulation module.
     *
     *  @param namestr (in) is the name of the module
     *  @param parent (in) is the parent of this module
     *  @param stack (in) is the size in bytes of the stack for this module
     */
    AbstractDisk();

    /**
     *  This is the destructor for this simulation module.
     */
    virtual ~AbstractDisk();

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

protected:
    
    // hook functions to (re)define behaviour
    virtual double service(cMessage *msg) = 0;

    cMessage *msgServiced;
    cMessage *endServiceMsg;
    cQueue queue;

    long current_block;
    long this_block;
    long this_distance;
    long total_distance;
    double start_time;
    double access_time;
    cOutVector *diskAccess;
    cStdDev *AccessStdDev;
    cStdDev *SeekStdDev;

    cMessage *cDiskDelayMessage;
    cMessage *cPendingRequestMessage;

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
class BasicModelDisk : public AbstractDisk
{
public:

    /** Initialize Omnet model */
    virtual void initialize();

protected:
    
    /** Concrete implementation of service method */
    virtual double service(cMessage* msg);

    virtual void handleMessage(cMessage* msg);
    
private:

    // Data descibing disk characteristics
    double fixedControllerReadOverheadSecs_;
    double fixedControllerWriteOverheadSecs_;
    double trackSwitchTimeSecs_;
    double averageReadSeekSecs_;
    double averageWriteSeekSecs_;
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

/**
 * @brief Fake disk with given service time
 */
class ACPDisk : public AbstractDisk
{
  public:
    /**
     *  This is the constructor for this simulation module.
     *
     *  @param namestr (in) is the name of the module
     *  @param parent (in) is the parent of this module
     *  @param stack (in) is the size in bytes of the stack for this module
     */
    ACPDisk();

    /**
     *  This is the destructor for this simulation module.
     */
    virtual double service(cMessage *msg);
};

/**
 *  @brief Model of Hewlett-Packard HP-97560 Disk Drive
 *
 *  The implementation is based on information found in the article:
 *
 *    "An Introduction to Disk Drive Modeling"
 *    Chris Ruemmler and John Wilkes
 *    IEEE Computer, March 1994, pp. 17-28
 *
 *  The purpose of this model is to model the basic characteristics of
 *  a real hard disk.  If the simulator were used to evaluate the
 *  impact of changing hard drive models in a given system, then
 *  getting the model to accurately model a real disk would be
 *  very important.  At this point, we just need a model that
 *  accounts for the basic architecture of a hard drive: rotating
 *  platters with multiple cylinders.
 *
 *  The following characteristics are modeled:
 *
 *      - Cylinders: 1,962
 *      - Tracks per Cylinder: 19
 *      - Data Sectors per Track: 72
 *      - Sector Size in bytes: 512
 *      - RPMs: 4,002
 *      - Interface: SCSI-II
 *      - Short Seek Time (<=383 cyls): 3.24 + 0.400 * sqrt(distance)
 *      - Long Seek Time (>383 cyls): 8.00 + 0.008 * distance
 *      - Fixed Controller Overhead
 *         - On Reads: 2.2 milliseconds
 *         - On Writes: 2.2 milliseconds
 *      - Track Switch Time: 1.6 milliseconds
 *
 *  This model currently ignores the following performance characteristics:
 *
 *      - Track Skew: 8 sectors
 *      - Cylinder Skew: 18 sectors
 *      - Read Fence Size: 64 Kilobytes
 *      - Sparing Algorithm: track
 *      - Disk Buffer Cache Size: 128 Kilobytes
 */
class HP97560Disk : public AbstractDisk
{
  // last request information
  long   last_cylinder;
  double last_time;

  public:
    /**
     *  This is the constructor for this simulation module.
     *
     *  @param namestr (in) is the name of the module
     *  @param parent (in) is the parent of this module
     *  @param stack (in) is the size in bytes of the stack for this module
     */
    HP97560Disk();

    virtual double service(cMessage *msg);
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


