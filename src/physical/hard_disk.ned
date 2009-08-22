//
// This file is part of Hecios
//
// Copyright (C) 2007 Brad Settlemyer
//
// This file is distributed WITHOUT ANY WARRANTY. See the file 'License.txt'
// for details on this and other legal matters.
//

// Abstract interface for all Hard Disks
simple HardDisk
{
    gates:
        input in;
        output out;
}

//
// Hard Disk Model that uses the simplified model examined in the FSS
// simulator
//
simple BasicModelDisk
{
    parameters:
        double fixedControllerReadOverheadSecs;
        double fixedControllerWriteOverheadSecs;
        double trackSwitchTimeSecs;
        double averageReadSeekSecs;
        double averageWriteSeekSecs;
        double numCylinders;
        double numHeads;
        double numSectors;
        double sectorsPerTrack;
        double rpm;

    gates:
        input in;
        output out;
}

