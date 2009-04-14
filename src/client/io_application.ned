
//
// Abstract interface for all I/O applications
//
simple IOApplication
	parameters:
        traceFile: string,
        maxBeginTime: numeric;
    gates:
        in: ioIn, mpiIn;
        out: ioOut, mpiOut;

endsimple

//
// An IOApplication using the PHTF trace file format
//
simple PHTFIOApplication
    parameters:
        disableCPUPhase: bool,
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
        traceFile: string,
        maxBeginTime: numeric;
    gates:
        in: ioIn, mpiIn;
        out: ioOut, mpiOut;

endsimple
