//-------------------------------------------------------------
// file: fsmodel.ned
//        (part of File System Simulator)
//
//  $Joel-Id: fsmodel.ned,v 1.4 1999/05/12 21:01:24 omnetpp Exp $
//-------------------------------------------------------------


//
// Abstract interface for File Systems
//
// Implemented File Systems:
// -  PassThroughFileSystem
//
simple FileSystem
  gates:
    in: in, response;
    out: out, request;
endsimple


