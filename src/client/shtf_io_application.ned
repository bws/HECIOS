simple SHTFIOApplication
    parameters:
        traceFile: string,
        numTraceProcs : const;

    gates:
        in: ioIn, mpiIn;
        out: ioOut, mpiOut;

endsimple
