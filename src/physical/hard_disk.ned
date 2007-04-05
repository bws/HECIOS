//-------------------------------------------------------------
// file: disk.ned
//        (part of File System)
//
//  $Joel-Id: disk.ned,v 1.4 1999/05/12 21:01:24 omnetpp Exp $
//-------------------------------------------------------------


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


