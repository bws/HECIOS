
//
// Abstract interface for all I/O applications
//
simple IOApplication
    parameters:
        traceFile: string,
        numTraceProcs : const;

    gates:
        in: ioIn, mpiIn;
        out: ioOut, mpiOut;

endsimple
