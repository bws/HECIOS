//
// This file is part of Hecios
//
// Copyright (C) 2006 Joel Sherrill <joel@oarcorp.com>
// Copyright (C) 2007 Brad Settlemyer
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
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
}

void HardDisk::finish()
{
    recordScalar("SPFS Disk Delay", totalDelay_);
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
    }
    else if (spfsOSWriteDeviceRequest* write =
             dynamic_cast<spfsOSWriteDeviceRequest*>(msg))
    {
        LogicalBlockAddress diskBlock = write->getAddress();
        delay = service(diskBlock, false);
        resp = new spfsOSWriteDeviceResponse();
    }
    else
    {
        cerr << "Error in Hard Disk Module!!!" << endl;
        assert(0);
    }

    // Update collection data
    //cerr << "Disk delay: " << delay <<endl;
    totalDelay_ += delay;
    
    // Schedule response at the end of service period
    resp->setContextPointer(msg);
    sendDelayed(resp, delay, "out");
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

    // Account for head switch time/fixed controller overhead
    if (isRead)
    {
      totalDelay += fixedControllerReadOverheadSecs_;
    }
    else
    {
      totalDelay += fixedControllerWriteOverheadSecs_;
    }

    // Account for the sector switching/rotational delay
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
            sectorsToMove  = sectorsPerTrack_ - currentSector;
            sectorsToMove += destSector;
        }
        totalDelay += sectorsToMove * timePerSector_;
    }
    
    // Add delay to transfer the data off the media
    totalDelay += timePerSector_;

    // Update disk state
    lastCylinder_ = destCylinder;
    lastHead_ = destHead;
    
    return totalDelay;
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
