
import
    "io_application",
    "middleware_cache",
    "fs_client",
    "bmi_transport";

//
// Compound module describing and MPI job process
//
module MPIProcess

    gates:

        in: netIn;
        out: netOut;

    submodules:

        app: IOApplication;

        cache: MiddlewareCache;

        fsClient: FSClient;

    connections:

        app.out --> cache.appIn;
        app.in <-- cache.appOut;

        cache.fsOut --> fsClient.appIn;
        cache.fsIn <-- fsClient.appOut;

        fsClient.netOut --> netOut;
        fsClient.netIn <-- netIn;

endmodule
