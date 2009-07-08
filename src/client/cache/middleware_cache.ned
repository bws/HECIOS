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
simple MiddlewareCache
	parameters:
		byteCopyTime: const;
		
    gates:
        in: appIn, fsIn, mpiIn;
        out: appOut, fsOut, mpiOut;
endsimple

//
// Passthrough/No-op middleware cache
//
simple NoMiddlewareCache
	parameters:
		byteCopyTime: const;
		
    gates:
        in: appIn, fsIn, mpiIn;
        out: appOut, fsOut, mpiOut;
endsimple

//
// Middleware cache using pages of direct cache
//
simple DirectPagedMiddlewareCache
    parameters:
    	byteCopyTime: const,
    	pageSize: const,
    	pageCapacity: const;
    	
	gates:
		in: appIn, fsIn, mpiIn;
        out: appOut, fsOut, mpiOut;
endsimple

//
// Middleware cache using pages of direct cache
//
simple SharedDirectPagedMiddlewareCache
    parameters:
    	byteCopyTime: const,
    	pageSize: const,
    	pageCapacity: const;
    	
	gates:
		in: appIn, fsIn, mpiIn;
        out: appOut, fsOut, mpiOut;
endsimple

//
// Middleware cache using pages of fully associative cache
//
simple FullyPagedMiddlewareCache
    parameters:
        pageSize: const,
        pageCapacity: const;
        
    gates:
        in: appIn, fsIn, mpiIn;
        out: appOut, fsOut, mpiOut;
endsimple

//
// Middleware cache using pages with Treadmarks twin diffing false sharing
// avoidance
//
simple PagedMiddlewareCacheWithTwinNoBlockIndexed
    parameters:
    	byteCopyTime: const,
    	pageSize: const,
    	pageCapacity: const;
    	
	gates:
		in: appIn, fsIn, mpiIn;
        out: appOut, fsOut, mpiOut;
endsimple

//
// Middleware cache using pages with Treadmarks twin diffing false sharing
// avoidance
//
simple PagedMiddlewareCacheWithTwin
    parameters:
    	byteCopyTime: const,
    	pageSize: const,
    	pageCapacity: const;
    	
	gates:
		in: appIn, fsIn, mpiIn;
        out: appOut, fsOut, mpiOut;
endsimple

//
// Middleware cache using pages with Treadmarks twin diffing false sharing
// avoidance
//
simple SharedPagedMiddlewareCacheWithTwin
    parameters:
    	byteCopyTime: const,
    	pageSize: const,
    	pageCapacity: const;
    	
	gates:
		in: appIn, fsIn, mpiIn;
        out: appOut, fsOut, mpiOut;
endsimple

//
// Middleware cache using pages with Treadmarks twin diffing false sharing
// avoidance
//
simple SharedPagedMiddlewareCacheWithTwinNoBlockIndexed
    parameters:
    	byteCopyTime: const,
    	pageSize: const,
    	pageCapacity: const;
    	
	gates:
		in: appIn, fsIn, mpiIn;
        out: appOut, fsOut, mpiOut;
endsimple

//
// Middleware cache using pages with Treadmarks twin diffing false sharing
// avoidance
//
simple PagedMiddlewareCacheMesi
    parameters:
    	byteCopyTime: const,
    	pageSize: const,
    	pageCapacity: const;
    	
	gates:
		in: appIn, fsIn, mpiIn;
        out: appOut, fsOut, mpiOut;
endsimple

//
// Middleware cache using pages with Treadmarks twin diffing false sharing
// avoidance
//
simple SharedPagedMiddlewareCacheMesi
    parameters:
    	byteCopyTime: const,
    	pageSize: const,
    	pageCapacity: const;
    	
	gates:
		in: appIn, fsIn, mpiIn;
        out: appOut, fsOut, mpiOut;
endsimple

//
// Middleware cache using pages of progressive paged cache
//
simple ProgressivePagedMiddlewareCache
    parameters:
    	byteCopyTime: const,
    	pageSize: const,
    	pageCapacity: const;
    	
	gates:
		in: appIn, fsIn, mpiIn;
        out: appOut, fsOut, mpiOut;
endsimple

//
// Middleware cache using shared pages of progressive paged cache
//
simple SharedProgressivePagedMiddlewareCache
    parameters:
    	byteCopyTime: const,
    	pageSize: const,
    	pageCapacity: const;
    	
	gates:
		in: appIn, fsIn, mpiIn;
        out: appOut, fsOut, mpiOut;
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
