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
// Template for I/O Library models
//
moduleinterface IOLibrary
{
    gates:
        input in;
        input response;
        output out;
        output request;
}

//
// Passthrough/No-op I/O library model
//
simple PassThroughIOLibrary like IOLibrary
{
	@class(PassThroughLibrary);
    gates:
        input in;
        input response;
        output out;
        output request;
}

//
// I/O Library model that supports List I/O messages
//
simple ListIOLibrary like IOLibrary
{
	@class(ListIOLibrary);
    gates:
        input in;
        input response;
        output out;
        output request;
}

//
// Local variables:
//  c-indent-level: 4
//  c-basic-offset: 4
// End:
//
// vim: ts=4 sts=4 sw=4 expandtab
//

