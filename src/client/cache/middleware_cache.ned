//
// This file is part of Hecios
//
// Copyright (C) 2008,2009 Brad Settlemyer
//
// This file is distributed WITHOUT ANY WARRANTY. See the file 'License.txt'
// for details on this and other legal matters.
//

//
// Template for middleware MPI-IO process caching
//
moduleinterface MiddlewareCache
{
    parameters:
        double byteCopyTime;

    gates:
        input appIn;
        input fsIn;
        input mpiIn;
        output appOut;
        output fsOut;
        output mpiOut;
}

//
// Passthrough/No-op middleware cache
//
simple NoMiddlewareCache like MiddlewareCache
{
    parameters:
		@class(NoMiddlewareCache);
        double byteCopyTime;

    gates:
        input appIn;
        input fsIn;
        input mpiIn;
        output appOut;
        output fsOut;
        output mpiOut;

}

//
// Middleware cache using pages of direct cache
//
simple DirectPagedMiddlewareCache like MiddlewareCache
{
    parameters:
		@class(DirectPagedMiddlewareCache);
        double byteCopyTime;
        double pageSize;
        double pageCapacity;

    gates:
        input appIn;
        input fsIn;
        input mpiIn;
        output appOut;
        output fsOut;
        output mpiOut;
}

//
// Middleware cache using pages of direct cache
//
simple SharedDirectPagedMiddlewareCache like MiddlewareCache
{
    parameters:
		@class(SharedDirectPagedMiddlewareCache);
        double byteCopyTime;
        double pageSize;
        double pageCapacity;

    gates:
        input appIn;
        input fsIn;
        input mpiIn;
        output appOut;
        output fsOut;
        output mpiOut;
}

//
// Middleware cache using pages of fully associative cache
//
simple FullyPagedMiddlewareCache like MiddlewareCache
{
    parameters:
		@class(FullyPagedMiddlewareCache);
        double byteCopyTime;
        double pageSize;
        double pageCapacity;

    gates:
        input appIn;
        input fsIn;
        input mpiIn;
        output appOut;
        output fsOut;
        output mpiOut;
}

//
// Middleware cache using pages with Treadmarks twin diffing false sharing
// avoidance
//
simple PagedMiddlewareCacheWithTwinNoBlockIndexed like MiddlewareCache
{
    parameters:
		@class(PagedMiddlewareCacheWithTwinNoBlockIndexed);
        double byteCopyTime;
        double pageSize;
        double pageCapacity;

    gates:
        input appIn;
        input fsIn;
        input mpiIn;
        output appOut;
        output fsOut;
        output mpiOut;
}

//
// Middleware cache using pages with Treadmarks twin diffing false sharing
// avoidance
//
simple PagedMiddlewareCacheWithTwin like MiddlewareCache
{
    parameters:
		@class(PagedMiddlewareCacheWithTwin);
        double byteCopyTime;
        double pageSize;
        double pageCapacity;

    gates:
        input appIn;
        input fsIn;
        input mpiIn;
        output appOut;
        output fsOut;
        output mpiOut;
}

//
// Middleware cache using pages with Treadmarks twin diffing false sharing
// avoidance
//
simple SharedPagedMiddlewareCacheWithTwin like MiddlewareCache
{
    parameters:
		@class(SharedPagedMiddlewareCacheWithTwin);
        double byteCopyTime;
        double pageSize;
        double pageCapacity;

    gates:
        input appIn;
        input fsIn;
        input mpiIn;
        output appOut;
        output fsOut;
        output mpiOut;
}

//
// Middleware cache using pages with Treadmarks twin diffing false sharing
// avoidance
//
simple SharedPagedMiddlewareCacheWithTwinNoBlockIndexed like MiddlewareCache
{
    parameters:
		@class(SharedPagedMiddlewareCacheWithTwinNoBlockIndexed);
        double byteCopyTime;
        double pageSize;
        double pageCapacity;

    gates:
        input appIn;
        input fsIn;
        input mpiIn;
        output appOut;
        output fsOut;
        output mpiOut;
}

//
// Middleware cache using pages with Treadmarks twin diffing false sharing
// avoidance
//
simple PagedMiddlewareCacheMesi like MiddlewareCache
{
    parameters:
		@class(PagedMiddlewareCacheMesi);
        double byteCopyTime;
        double pageSize;
        double pageCapacity;

    gates:
        input appIn;
        input fsIn;
        input mpiIn;
        output appOut;
        output fsOut;
        output mpiOut;
}

//
// Middleware cache using pages with Treadmarks twin diffing false sharing
// avoidance
//
simple SharedPagedMiddlewareCacheMesi like MiddlewareCache
{
    parameters:
		@class(SharedPagedMiddlewareCacheMesi);
        double byteCopyTime;
        double pageSize;
        double pageCapacity;

    gates:
        input appIn;
        input fsIn;
        input mpiIn;
        output appOut;
        output fsOut;
        output mpiOut;
}

//
// Middleware cache using pages of progressive paged cache
//
simple ProgressivePagedMiddlewareCache like MiddlewareCache
{
    parameters:
	 	@class(ProgressivePagedMiddlewareCache);
        double byteCopyTime;
        double pageSize;
        double pageCapacity;

    gates:
        input appIn;
        input fsIn;
        input mpiIn;
        output appOut;
        output fsOut;
        output mpiOut;
}

//
// Middleware cache using shared pages of progressive paged cache
//
simple SharedProgressivePagedMiddlewareCache like MiddlewareCache
{
    parameters:
		@class(SharedProgressivePagedMiddlewareCache);
        double byteCopyTime;
        double pageSize;
        double pageCapacity;

    gates:
        input appIn;
        input fsIn;
        input mpiIn;
        output appOut;
        output fsOut;
        output mpiOut;
}

