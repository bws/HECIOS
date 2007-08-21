#ifndef HARD_DISK_H
#define HARD_DISK_H
//
// This file is part of Hecios
//
// Copyright (C) 2004 Joel Sherrill <joel@oarcorp.com>
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

private:
    
    /** @return the disk service time for the message */
    virtual double service(LogicalBlockAddress blockNumber, bool isRead) = 0;

    /** @return the basic block size for the disk model */
    virtual uint32_t basicBlockSize() const = 0;
    
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
    uint32_t headsPerCylinder_;
    uint32_t tracksPerCylinder_;
    uint32_t sectorsPerTrack_;
    uint32_t rpms_;

    // Data derived from disk characteristics
    uint32_t sectorsPerCylinder_;
    uint32_t numSectors_;
    double timePerRevolution_;
    double timePerSector_;

    // Disk state
    uint32_t lastCylinder_;
    uint32_t lastHead_;
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


