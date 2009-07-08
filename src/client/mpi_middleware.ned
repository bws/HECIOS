//
// This file is part of Hecios
//
// Copyright (C) 2008,2009 Brad Settlemyer
//
// This file is distributed WITHOUT ANY WARRANTY. See the file 'License.txt'
// for details on this and other legal matters.
//

//
// MPI middleware
//

simple MpiMiddleware
    gates:
        in: appIn, aggIn, cacheIn, netIn;
        out: appOut, aggOut, cacheOut, netOut;
endsimple


//
// Local variables:
//  indent-tabs-mode: nil
//  c-indent-level: 4
//  c-basic-offset: 4
// End:
//
// vim: ts=4 sts=4 sw=4 expandtab
//
