//-------------------------------------------------------------
// file: syscall.ned
//        (part of File System Simulator)
//
//  $Joel-Id: syscall.ned,v 1.2 1999/05/12 21:01:25 omnetpp Exp $
//-------------------------------------------------------------


//
// Abstract interface for System Call Interfaces
//
// Implemented System Call Interfaces:
// -  PassThroughSystemCallInterface
//
simple ASystemCallInterface
  gates:
    in: in, response;
    out: out, request;
endsimple


