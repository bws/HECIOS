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
simple BufferCache
    gates:
        in: in, response;
        out: out, request;
endsimple

//
// Passthrough/No-op buffer cache
//
simple NoBufferCache
    gates:
        in: in, response;
        out: out, request;
endsimple

//
// BufferCache using an LRU replacement policy
//
simple LRUBufferCache
    parameters:
        numEntries : const,
        dirtyThreshold: const;

    gates:
        in: in, response;
        out: out, request;
endsimple

//
// Local variables:
//  c-indent-level: 4
//  c-basic-offset: 4
// End:
//
// vim: ts=4 sts=4 sw=4 expandtab
//
