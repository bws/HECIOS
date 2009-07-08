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
    gates:
        in: in;
        out: out;
endsimple

//
// Hard Disk Model that uses the simplified model examined in the FSS
// simulator
//
simple BasicModelDisk
    parameters:
        fixedControllerReadOverheadSecs: const,
        fixedControllerWriteOverheadSecs: const,
        trackSwitchTimeSecs: const,
        averageReadSeekSecs: const,
        averageWriteSeekSecs: const,
        numCylinders: const,
        numHeads: const,
        numSectors: const,
        sectorsPerTrack: const,
        rpm: const;

    gates:
        in: in;
        out: out;
endsimple  

//
// Local variables:
//  indent-tabs-mode: nil
//  c-indent-level: 4
//  c-basic-offset: 4
// End:
//
// vim: ts=4 sts=4 sw=4 expandtab foldmethod=marker
//
