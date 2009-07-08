//
// This file is part of Hecios
//
// Copyright (C) 2009 Brad Settlemyer
//
// This file is distributed WITHOUT ANY WARRANTY. See the file 'License.txt'
// for details on this and other legal matters.
//

//
// Template for middleware MPI-IO process aggregator
//
simple MiddlewareAggregator
    gates:
        in: appIn, ioIn, mpiIn;
        out: appOut, ioOut, mpiOut;
endsimple

//
// Passthrough/No-op middleware cache
//
simple NoMiddlewareAggregator
    gates:
        in: appIn, ioIn, mpiIn;
        out: appOut, ioOut, mpiOut;
endsimple

//
// Middleware cache using pages of direct cache
//
simple DataSievingMiddlewareAggregator
	gates:
		in: appIn, ioIn, mpiIn;
        out: appOut, ioOut, mpiOut;
endsimple

//
// Middleware cache using pages of direct cache
//
simple ViewAwareMiddlewareAggregator
	gates:
		in: appIn, ioIn, mpiIn;
        out: appOut, ioOut, mpiOut;
endsimple

//
// Local variables:
//  indent-tabs-mode: nil
//  c-indent-level: 4
//  c-basic-offset: 4
// End:
//
// vim: ts=4 sts=4 sw=4 expandtab
//
