
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

        in: netIn;
        out: netOut;

    submodules:

        app: IOApplication;
            display: "p=80,80;i=block/app,white";

        //cache: MiddlewareCache;
        cache: cacheModule;
            display: "p=80,180;i=abstract/cache,gold";

        fsClient: fsModule;
            display: "p=80,280;o=orange red";

    connections:

        app.out --> cache.appIn;
        app.in <-- cache.appOut;

        cache.fsOut --> fsClient.appIn;
        cache.fsIn <-- fsClient.appOut;

        fsClient.netOut --> netOut;
        fsClient.netIn <-- netIn;

    display: "o=white";

endmodule
