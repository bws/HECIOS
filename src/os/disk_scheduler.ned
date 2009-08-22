//
// This file is part of Hecios
//
// Copyright (C) 2006 Joel Sherrill <joel@oarcorp.com>
// Copyright (C) 2007,2008,2009 Brad Settlemyer
//
// This file is distributed WITHOUT ANY WARRANTY. See the file 'License.txt'
// for details on this and other legal matters.
//

//
// Abstract interface for Disk Schedulers
//
// Implemented Disk Schedulers:
// -  FIFOScheduler: FIFO Disk Scheduler
// -  SSTFScheduler: SSTF Disk Scheduler
// -  CScanScheduler: CScan Disk Scheduler
// -  NStepCScanScheduler: NStep Scan Disk Scheduler
// -  ScanScheduler: Scan Disk Scheduler
// -  NStepScanScheduler: NStep Scan Disk Scheduler
//
moduleinterface DiskScheduler
{
    gates:
        input in;
        input response;
        output out;
        output request;
}

simple FCFSDiskScheduler like DiskScheduler
{
	@class(FCFSDiskScheduler);
    gates:
        input in;
        input response;
        output out;
        output request;
}

simple SSTFDiskScheduler like DiskScheduler
{
	@class(SSTFDiskScheduler);
    gates:
        input in;
        input response;
        output out;
        output request;
}

simple ScanDiskScheduler like DiskScheduler
{
	@class(ScanDiskScheduler);
    gates:
        input in;
        input response;
        output out;
        output request;
}

simple CScanDiskScheduler like DiskScheduler
{
	@class(CScanDiskScheduler);
    gates:
        input in;
        input response;
        output out;
        output request;
}

simple NStepDiskScheduler like DiskScheduler
{
	@class(NStepDiskScheduler);
    gates:
        input in;
        input response;
        output out;
        output request;
}

simple NStepCScanDiskScheduler like DiskScheduler
{
	@class(NStepCScanDiskScheduler);
    gates:
        input in;
        input response;
        output out;
        output request;
}


