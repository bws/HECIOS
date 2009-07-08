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
simple FileSystem
    parameters:
        noATime : bool;

    gates:
        in: in, response;
        out: out, request;
endsimple

//
// A UNIX/Linux file system model
//
simple NativeFileSystem
    parameters:
        blockSizeBytes : const,
        noATime: bool;

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
