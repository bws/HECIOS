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
simple DiskScheduler
  gates:
     in: in, response;
     out: out, request;
endsimple


