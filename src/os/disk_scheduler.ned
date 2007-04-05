//-------------------------------------------------------------
// file: scheduler.ned
//        (part of File System Simulator)
//
//  $Joel-Id: scheduler.ned,v 1.17 1999/05/12 21:01:25 omnetpp Exp $
//-------------------------------------------------------------


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
// -  PriorityScheduler: Priority Disk Scheduler
// -  PriorityScanScheduler: Priority Scan Disk Scheduler
// -  PriorityCScanScheduler: Priority CScan Disk Scheduler
// -  PriorityNStepScanScheduler: Priority N-Step Scan Disk Scheduler
// -  FairShareScheduler: Fair Share Disk Scheduler
// -  PreemptivePriorityNStepScanScheduler: Priority N-Step Scan Disk Scheduler
//                                          with Preemption
// -  PreemptiveFairShareScheduler: Preemptive Fair Share Disk Scheduler
//
simple ADiskScheduler
  parameters:
     scheduler_file: string,
     number_of_ops, period;
  gates:
     in: in, get_next_request, response;
     out: out, next_request;
endsimple


