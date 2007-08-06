
#include <cassert>
#include <cstdlib>
#include "hard_disk.h"
using namespace std;

// Register the C++ class for the NED module
//Define_Module(HardDisk);

HardDisk::HardDisk()
    : cSimpleModule()
{
}

HardDisk::~HardDisk()
{
}

void HardDisk::initialize()
{
}

void HardDisk::finish()
{
}

void HardDisk::handleMessage(cMessage *msg)
{
    if (msg->isSelfMessage())
    {
        send(msg, "out");
    }
    else
    {
        long long diskBlock = msg->par("block").longValue();
        bool isRead = msg->par("is_read").boolValue();
        
        //simtime_t requestArrivalTime = simTime();
        double serviceTime = service(diskBlock, isRead);
        scheduleAt(serviceTime, msg);
    }    
}

long HardDisk::getBasicBlockSize() const
{
    long size = basicBlockSize();
    assert(0 == (size % 2));
    return size;
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
    fixedControllerReadOverheadSecs_ =
        par("fixedControllerReadOverheadSecs").doubleValue();
    fixedControllerWriteOverheadSecs_ =
        par("fixedControllerWriteOverheadSecs").doubleValue();
    trackSwitchTimeSecs_ = par("trackSwitchTimeSecs").doubleValue();
    averageReadSeekSecs_ = par("averageReadSeekSecs").doubleValue();
    averageWriteSeekSecs_ = par("averageWriteSeekSecs").doubleValue();

    // FIXME: Need workaround to get 64 bits of integer precisison
    // could use strings here, but double reinterpret won't work :(
    capacity_ = par("capacity").longValue();
    
    numCylinders_ = par("numCylinders").longValue();
    tracksPerCylinder_ = par("tracksPerCylinder").longValue();
    sectorsPerTrack_ = par("sectorsPerTrack").longValue();
    rpms_ = par("rpms").longValue();

    // Calculated data
    sectorsPerCylinder_ = sectorsPerTrack_ * tracksPerCylinder_;
    numSectors_ = sectorsPerCylinder_ * numCylinders_;
    timePerRevolution_ = 1.0 / rpms_;
    timePerSector_ = timePerRevolution_ / sectorsPerCylinder_;
}

double BasicModelDisk::service(long long blockNumber, bool isRead)
{
    // Service delay
    double totalDelay = 0.0;

    // Determine physical destination 
    long destBlock = blockNumber;
    long destCylinder = destBlock / sectorsPerCylinder_;
    long destSector = destBlock % sectorsPerCylinder_;

    // Account for fixed controller overhead
    if (isRead)
    {
      totalDelay += fixedControllerReadOverheadSecs_;
    }
    else
    {
      totalDelay += fixedControllerWriteOverheadSecs_;
    }

    // get to the right cylinder
    long cylindersToMove = abs(destCylinder - lastCylinder_);
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

    // Account for the rotational delay
    long sectorsToMove = 0;
    long currentSector =
        static_cast<long>(fmod(simTime(), timePerRevolution_)/timePerSector_);
    if ( currentSector != destSector )
    {
        if ( currentSector < destSector )
        {
            sectorsToMove = destSector - currentSector;
        }
        else
        {
            // Must wrap around to sector 0
            sectorsToMove  = sectorsPerCylinder_ - currentSector;
            sectorsToMove += destSector;
        }
        totalDelay += sectorsToMove * timePerSector_;
    }
    
    // Account for movement between tracks
    if ( sectorsToMove > 0 )
    {
        long currentTrack = currentSector / sectorsPerTrack_;
        long destTrack = destSector / sectorsPerTrack_;
        if ( currentTrack != destTrack )
        {
            long numTracks = abs(destTrack - currentTrack);
            totalDelay += numTracks * trackSwitchTimeSecs_;
        }
    }

    // Add delay to transfer the data off the media
    totalDelay += timePerSector_;

    // Update disk state
    lastCylinder_ = destCylinder;
    
    return totalDelay;
}

long BasicModelDisk::basicBlockSize() const
{
    //FIXME return capacity_ / numSectors_;
    return 512;
}

/*
 * Local variables:
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 *
 * vim: ts=8 sts=4 sw=4 expandtab
 */
