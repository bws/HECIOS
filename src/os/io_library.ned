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
simple IOLibrary
    gates:
        in: in, response;
        out: out, request;
endsimple

//
// Passthrough/No-op I/O library model
//
simple PassThroughIOLibrary
    gates:
        in: in, response;
        out: out, request;
endsimple

//
// I/O Library model that supports List I/O messages
//
simple ListIOLibrary
  gates:
    in: in, response;
    out: out, request;
endsimple

//
// Local variables:
//  c-indent-level: 4
//  c-basic-offset: 4
// End:
//
// vim: ts=4 sts=4 sw=4 expandtab
//

