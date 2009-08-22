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
// Abstract interface for Disk Access Managers
//
// Implemented Access Managers:
// -  SimpleAccessManager: passes through all requests
// -  MutexAccessManager: only one outstanding request at a time
//
simple AccessManager
{
    gates:
        input next_request;
        input block;
        output get_next_request;
        output get_block;
        output out;
}

simple SimpleAccessManager extends AccessManager
{
    @class(SimpleAccessManager);
}

simple MutexAccessManager extends AccessManager
{
    @class(MutexAccessManager);
}

