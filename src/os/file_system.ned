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
// Template for File Systems
//
moduleinterface FileSystem
{
    gates:
        input in;
        input response;
        output out;
        output request;
}

//
// A UNIX/Linux file system model
//
simple NativeFileSystem like FileSystem
{
	@class(NativeFileSystem);
    double blockSizeBytes;
    bool noATime;

    gates:
        input in;
        input response;
        output out;
        output request;
}

