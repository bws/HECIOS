//-------------------------------------------------------------
// file: cache.ned
//        (part of File System Simulator)
//
//  $Joel-Id: cache.ned,v 1.9 1999/05/12 21:01:24 omnetpp Exp $
//-------------------------------------------------------------


//
// Abstract interface for Disk Cache Managers
//
// Implemented Cache Managers:
// -  NoCache : all requests are passed through to the Disk Subsystem
// -  FIFOCache : FIFO Cache Manager
// -  LRUCache : Least Recently Used Cache Manager
// -  PriorityLRUCache : Priority LRU Cache Manager
// -  FairShareCache : Fair Share Cache Manager
// -  PIPriorityLRUCache : Priority LRU Cache Manager with Priority Inheritance
// -  PIFairShareCache : Fair Share with Priority Inheritance Cache Manager
//
simple ACache
  parameters:
    cache_file: string,
    number_of_elements;
  gates:
    in: in, response;
    out: out, request;
endsimple


