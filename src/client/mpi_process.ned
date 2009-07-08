//
// This file is part of Hecios
//
// Copyright (C) 2007,2008,2009 Brad Settlemyer
//
// This file is distributed WITHOUT ANY WARRANTY. See the file 'License.txt'
// for details on this and other legal matters.
//

import
    "io_application",
    "cache_module",
    "fs_client",
    "bmi_transport",
    "mpi_middleware",
    "cacheModule";

//
// Compound module describing and MPI job process
//
module MPIProcess
    parameters:
        middlewareAggregatorType : string,
        middlewareCacheType : string,
        IOApplicationType : string;

    gates:
        in: ioIn, netIn;
        out: ioOut, netOut;

    submodules:
        app:IOApplicationType like IOApplication;
            display: "p=150,80;i=block/app,white";

		mpiMiddleware: MpiMiddleware;
	    	display: "p=60,180;i=block/classifier,red";

        aggregator: middlewareAggregatorType like MiddlewareAggregator;
            display: "p=240,180;i=block/join,yellow";

        cache: middlewareCacheType like MiddlewareCache;
            display: "p=150,280;i=block/buffer2,green";

    connections:
        // Connect the I/O application to the MPI Middleware
		app.mpiOut --> mpiMiddleware.appIn;
		app.mpiIn <-- mpiMiddleware.appOut;

        // Connect the Aggregator to the MPI Middleware
		aggregator.mpiOut --> mpiMiddleware.aggIn;
		aggregator.mpiIn <-- mpiMiddleware.aggOut;

        // Connect the Cache to the MPI Middleware
		cache.mpiOut --> mpiMiddleware.cacheIn;
		cache.mpiIn <-- mpiMiddleware.cacheOut;

        // Connect the MPI application to the aggregator middleware
        app.ioOut --> aggregator.appIn;
        app.ioIn <-- aggregator.appOut;
        
        // Connect the aggregator to the cache middleware
        aggregator.ioOut --> cache.appIn;
        aggregator.ioIn <-- cache.appOut;
        
        // Connect the Cache to the I/O subsystem
		cache.fsOut --> ioOut;
		cache.fsIn <-- ioIn;

        // Connect the MPI Middleware to the network
        mpiMiddleware.netOut --> netOut;
        mpiMiddleware.netIn <-- netIn;

    display: "o=white";
endmodule
