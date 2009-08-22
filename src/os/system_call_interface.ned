//
// This file is part of Hecios
//
// Copyright (C) 2006 Joel Sherrill <joel@oarcorp.com>
// Copyright (C) 2007,2008,2009 Brad Settlemyer
//
// This file is distributed WITHOUT ANY WARRANTY. See the file 'License.txt'
// for details on this and other legal matters.
//

//
// Abstract interface for System Call Interfaces
//
moduleinterface SystemCallInterface
{
    gates:
        input in;
        input response;
        output out;
        output request;
}

simple PassThroughSystemCallInterface like SystemCallInterface
{
	@class(PassThroughSystemCallInterface);
    double addReadOverheadSecs;
    double addWriteOverheadSecs;
    double overheadSecs;

    gates:
        input in;
        input response;
        output out;
        output request;
}

simple SequentialSystemCallInterface like SystemCallInterface
{
	@class(SequentialSystemCallInterface);
    double addReadOverheadSecs;
    double addWriteOverheadSecs;
    double overheadSecs;

    gates:
        input in;
        input response;
        output out;
        output request;
}


