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
moduleinterface IOApplication
{
    parameters:
        string traceFile;
        volatile double maxBeginTime;
    gates:
        input ioIn;
        input mpiIn;
        output ioOut;
        output mpiOut;

}

//
// An IOApplication using the PHTF trace file format
//
simple PHTFIOApplication like IOApplication
{
    parameters:
        bool disableCPUPhase;
        bool isVerbose;
        string traceFile;
        volatile double maxBeginTime;
    gates:
        input ioIn;
        input mpiIn;
        output ioOut;
        output mpiOut;

}

//
// An IOApplication using the SHTF trace file format
//
simple SHTFIOApplication like IOApplication
{
    parameters:
        string traceFile;
        volatile double maxBeginTime;
    gates:
        input ioIn;
        input mpiIn;
        output ioOut;
        output mpiOut;

}
