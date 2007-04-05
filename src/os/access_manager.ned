//-------------------------------------------------------------
// file: accessman.ned
//        (part of File System Simulator)
//
//  $Joel-Id: accessman.ned,v 1.5 1999/05/12 21:01:24 omnetpp Exp $
//-------------------------------------------------------------


//
// Abstract interface for Disk Access Managers
//
// Implemented Access Managers:
// -  SimpleAccessManager: passes through all requests
// -  MutexAccessManager: only one outstanding request at a time
//
simple AAccessManager
  gates:
    in: next_request, block;
    out: get_next_request, get_block, out;
endsimple


