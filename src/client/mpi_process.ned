
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

        //cache: MiddlewareCache;
        cache: cacheModule;

        fsClient: fsModule;

    connections:

        app.out --> cache.appIn;
        app.in <-- cache.appOut;

        cache.fsOut --> fsClient.appIn;
        cache.fsIn <-- fsClient.appOut;

        fsClient.netOut --> netOut;
        fsClient.netIn <-- netIn;

endmodule
