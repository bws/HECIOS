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
{
    gates:
        input appIn;
        input aggIn;
        input cacheIn;
        input netIn;
        output appOut;
        output aggOut;
        output cacheOut;
        output netOut;
}


