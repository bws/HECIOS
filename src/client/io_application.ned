
//
// Abstract interface for all I/O applications
//
simple IOApplication
    gates:
        in: ioIn, mpiIn;
        out: ioOut, mpiOut;

endsimple

//
// An IOApplication using the PHTF trace file format
//
simple PHTFIOApplication
    parameters:
        traceFile: string,
        maxBeginTime: numeric;
    gates:
        in: ioIn, mpiIn;
        out: ioOut, mpiOut;

endsimple

//
// An IOApplication using the SHTF trace file format
//
simple SHTFIOApplication
    parameters:
        traceFile: string;

    gates:
        in: ioIn, mpiIn;
        out: ioOut, mpiOut;

endsimple
