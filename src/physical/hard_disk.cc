//
// This file is part of Hecios
//
// Copyright (C) 2007,2008,2009 Brad Settlemyer
//
// This file is distributed WITHOUT ANY WARRANTY. See the file 'License.txt'
// for details on this and other legal matters.
//
#include "hard_disk.h"
#include <cassert>
#include <cstdlib>
#include "basic_types.h"
#include "os_proto_m.h"
using namespace std;

HardDisk::HardDisk()
{
}

HardDisk::~HardDisk()
{
}

void HardDisk::initialize()
{
    // Initialize collection data
    totalDelay_ = 0;
    totalBlocksRead_ = 0;
    totalBlocksWritten_ = 0;

    // Retrieve gate id
    outGateId_ = findGate("out");
}

void HardDisk::finish()
{
    recordScalar("SPFS Disk Delay", totalDelay_);
    recordScalar("SPFS Disk Blocks Read", totalBlocksRead_);
    recordScalar("SPFS Disk Blocks Written", totalBlocksWritten_);
}

void HardDisk::handleMessage(cMessage *msg)
{
    // Service and construct responses for read and write requests
    double delay = 0.0;
    cMessage* resp = 0;
    if (spfsOSReadDeviceRequest* read =
        dynamic_cast<spfsOSReadDeviceRequest*>(msg))
    {
        LogicalBlockAddress diskBlock = read->getAddress();
        delay = service(diskBlock, true);
        resp = new spfsOSReadDeviceResponse();
        totalBlocksRead_++;
    }
    else if (spfsOSWriteDeviceRequest* write =
             dynamic_cast<spfsOSWriteDeviceRequest*>(msg))
    {
        LogicalBlockAddress diskBlock = write->getAddress();
        delay = service(diskBlock, false);
        resp = new spfsOSWriteDeviceResponse();
        totalBlocksWritten_++;
    }
    else
    {
        cerr << __FILE__ << ":" << __LINE__ << ":"
             << "ERROR in hard disk for message:" << msg->info() << endl;
        assert(0);
    }

    // Schedule response at the end of service period
    resp->setContextPointer(msg);
    sendDelayed(resp, delay, outGateId_);
}

long HardDisk::getBasicBlockSize() const
{
    long size = basicBlockSize();
    assert(0 == (size % 2));
    return size;
}

void HardDisk::registerDiskDelay(double diskTime)
{
    // Update collection data
    totalDelay_ += diskTime;
    //cerr << __FILE__ << ":" << __LINE__ << ":"
    //     << "Disk delay: " << diskTime << " Total Delay: " << totalDelay_ << endl;
}

//
// Create BasicModelDisk module -- this not code originally developed as part
// of FSS
//
Define_Module(BasicModelDisk);

void BasicModelDisk::handleMessage(cMessage* msg)
{
    HardDisk::handleMessage(msg);
}

void BasicModelDisk::initialize()
{
    // Initialize parent
    HardDisk::initialize();

    // Drive layout parameters
    numCylinders_ = par("numCylinders").longValue();
    numHeads_ = par("numHeads").longValue();
    numSectors_ = par("numSectors").longValue();
    sectorsPerTrack_ = par("sectorsPerTrack").longValue();
    rpms_ = par("rpm").longValue();

    // Derived layout parameters
    headsPerCylinder_ = numCylinders_ / numHeads_;

    // Drive performance parameters
    fixedControllerReadOverheadSecs_ =
        par("fixedControllerReadOverheadSecs").doubleValue();
    fixedControllerWriteOverheadSecs_ =
        par("fixedControllerWriteOverheadSecs").doubleValue();
    trackSwitchTimeSecs_ = par("trackSwitchTimeSecs").doubleValue();
    averageReadSeekSecs_ = par("averageReadSeekSecs").doubleValue();
    averageWriteSeekSecs_ = par("averageWriteSeekSecs").doubleValue();

    // Derived performance parameters
    timePerRevolution_ = 1.0 / rpms_;
    timePerSector_ = timePerRevolution_ / sectorsPerTrack_;

    // FIXME: Need workaround to get 64 bits of integer precisison
    // could use strings here, but double reinterpret won't work :(
    capacity_ = 512 * numSectors_;

    // Park the head at the central cylinder to begin with
    lastCylinder_ = numCylinders_/2;

    // Set the time of the last service completion
    lastCompletionTime_ = 0.0;
}

double BasicModelDisk::service(LogicalBlockAddress blockNumber, bool isRead)
{
    // Service delay
    double totalDelay = 0.0;

    // Determine physical location, better would be to use zoned sector per
    // track counts
    int temp = blockNumber % (headsPerCylinder_ * sectorsPerTrack_);
    int destCylinder = blockNumber / (headsPerCylinder_ * sectorsPerTrack_);
    int destHead = temp / sectorsPerTrack_;
    int destSector = temp % sectorsPerTrack_ + 1;

    // Account for cylinder switch/arm movement
    int cylindersToMove = destCylinder - lastCylinder_;
    if (0 != cylindersToMove)
    {
        // Simply use the average seek time to calculate cylinder location
        // (a bad choice, but arm acceleration data is not currently available
        // this is a serious departure from the paper's model, possibly leading
        // to double digit inaccuracies)
        if (isRead)
            totalDelay += averageReadSeekSecs_;
        else
            totalDelay += averageWriteSeekSecs_;
    }

    // Account for head switch time/fixed controller overhead if the
    // controller is not currently active
    if (isRead)
    {
        totalDelay += fixedControllerReadOverheadSecs_;
    }
    else
    {
        totalDelay += fixedControllerWriteOverheadSecs_;
    }

    // Account for the sector switching/rotational delay
    simtime_t currentTime = simTime();
    long sectorsToMove = 0;
    long currentSector =
        static_cast<long>(fmod(currentTime.dbl(), timePerRevolution_)/timePerSector_);
    if (currentSector != destSector)
    {
        if (currentSector < destSector)
        {
            sectorsToMove = destSector - currentSector;
        }
        else
        {
            // Must wrap around to sector 0
            sectorsToMove  = sectorsPerTrack_ - currentSector;
            sectorsToMove += destSector;
        }
        totalDelay += sectorsToMove * timePerSector_;
    }

    // Add delay to transfer the data off the media
    totalDelay += timePerSector_;
    registerDiskDelay(totalDelay);

    // Update disk state
    lastCylinder_ = destCylinder;
    lastHead_ = destHead;
    lastCompletionTime_ = max(lastCompletionTime_, currentTime) + totalDelay;

    // Modify the delay to take into account that the disk can only service
    // one request at a time
    simtime_t completionDelay = (lastCompletionTime_ - currentTime) + totalDelay;
    return completionDelay.dbl();
}

uint32_t BasicModelDisk::basicBlockSize() const
{
    //FIXME return capacity_ / numSectors_;
    return 512;
}

/*
 * Local variables:
 *  indent-tabs-mode: nil
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 *
 * vim: ts=8 sts=4 sw=4 expandtab
 */
