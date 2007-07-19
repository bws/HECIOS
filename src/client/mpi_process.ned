
import
    "io_application",
    "cache_module",
    "fs_client",
    "bmi_transport",
    "cacheModule";

//
// Compound module describing and MPI job process
//
module MPIProcess

    gates:

        in: ioIn, mpiClientIn, mpiServerIn;
        out: ioOut, mpiClientOut, mpiServerOut;

    submodules:

        app: IOApplication;
            display: "p=80,80;i=block/app,white";

        //cache: MiddlewareCache;
        cache: cacheModule;
            display: "p=80,180;i=block/buffer2,green";

        fsClient: fsModule;
            display: "p=80,280;i=block/classifier,yellow";

    connections:

        // Connect the MPI application to the MPI messaging network
        app.mpiClientOut --> mpiClientOut;
        app.mpiClientIn <-- mpiClientIn;
        app.mpiServerOut --> mpiServerOut;
        app.mpiServerIn <-- mpiServerIn;

        // Connect the MPI application to the cache
        app.ioOut --> cache.appIn;
        app.ioIn <-- cache.appOut;

        // Connect the file cache to the file system client
        cache.fsOut --> fsClient.appIn;
        cache.fsIn <-- fsClient.appOut;

        // Connect the file system client to the I/O network
        fsClient.netOut --> ioOut;
        fsClient.netIn <-- ioIn;

    display: "o=white";

endmodule
