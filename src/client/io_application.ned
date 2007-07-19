
//
// Abstract interface for all I/O applications
//
simple IOApplication
    parameters:
        traceFile: string,
        numTraceProcs : const;

    gates:
        in: ioIn, mpiClientIn, mpiServerIn;
        out: ioOut, mpiClientOut, mpiServerOut;
endsimple
