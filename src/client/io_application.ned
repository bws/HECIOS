//
// This file is part of Hecios
//
// Copyright (C) 2007,2008,2009 Brad Settlemyer
//
// This file is distributed WITHOUT ANY WARRANTY. See the file 'License.txt'
// for details on this and other legal matters.
//

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
        isVerbose: bool,
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
