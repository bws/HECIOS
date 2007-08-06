//-------------------------------------------------------------
// file: disk.ned
//        (part of File System)
//
//  $Joel-Id: disk.ned,v 1.4 1999/05/12 21:01:24 omnetpp Exp $
//-------------------------------------------------------------

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
        capacity: const,
        numCylinders: const,
        tracksPerCylinder: const,
        sectorsPerTrack: const,
        rpms: const;

    gates:
        in: in;
        out: out;
endsimple  

// This model is from FSS, and is a place holder while figuring out what
// parts of the FSS system are usable and what is not
//
// Abstract interface for Physical Disks
//
// Implemented Physical Disks:
// -  ACPDisk : Fake disk with given service time
// -  HP97560Disk : Model of Hewlett-Packard HP-97560 Disk Drive
//
simple APhysicalDisk
  parameters:
    service_time;
  gates:
    in: in;
    out: out;
endsimple


//
// Local variables:
//  c-indent-level: 4
//  c-basic-offset: 4
// End:
//
// vim: ts=4 sts=4 sw=4 expandtab foldmethod=marker
//
