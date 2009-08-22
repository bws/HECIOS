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
moduleinterface MiddlewareAggregator
{
    gates:
        input appIn;
        input ioIn;
        input mpiIn;
        output appOut;
        output ioOut;
        output mpiOut;
}

//
// Passthrough/No-op middleware aggregator
//
simple NoMiddlewareAggregator like MiddlewareAggregator
{
	@class(NoMiddlewareAggregator);
    gates:
        input appIn;
        input ioIn;
        input mpiIn;
        output appOut;
        output ioOut;
        output mpiOut;
}

//
// Middleware aggregator providing data sieving
//
simple DataSievingMiddlewareAggregator like MiddlewareAggregator
{
	@class(DataSievingMiddlewareAggregator);
    gates:
        input appIn;
        input ioIn;
        input mpiIn;
        output appOut;
        output ioOut;
        output mpiOut;
}

//
// Middleware aggregator with view awareness
//
simple ViewAwareMiddlewareAggregator like MiddlewareAggregator
{
    @class(ViewAwareMiddlewareAggregator);
    gates:
        input appIn;
        input ioIn;
        input mpiIn;
        output appOut;
        output ioOut;
        output mpiOut;
}

