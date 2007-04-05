//-------------------------------------------------------------
// file: iolibrary.ned
//        (part of File System Simulator)
//
//  $Joel-Id: iolibrary.ned,v 1.2 1999/05/12 21:01:24 omnetpp Exp $
//-------------------------------------------------------------


//
// Abstract interface for I/O Library
//
// Implemented I/O Libraries:
// -  PassThroughIOLibrary
//
simple AIOLibrary
  gates:
    in: in, response;
    out: out, request;
endsimple


