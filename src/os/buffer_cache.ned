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
// Template for OS Block device buffer caches
//
moduleinterface BufferCache
{
    gates:
        input in;
        input response;
        output out;
        output request;
}

//
// Passthrough/No-op buffer cache
//
simple NoBufferCache like BufferCache
{
	@class(NoBufferCache);
    gates:
        input in;
        input response;
        output out;
        output request;
}

//
// BufferCache using an LRU replacement policy
//
simple LRUBufferCache like BufferCache
{
	@class(LRUBufferCache);
	double numEntries;
    double dirtyThreshold;

    gates:
        input in;
        input response;
        output out;
        output request;
}

