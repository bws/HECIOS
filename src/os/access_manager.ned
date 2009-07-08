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
  gates:
    in: next_request, block;
    out: get_next_request, get_block, out;
endsimple

//
// Local variables:
//  c-indent-level: 4
//  c-basic-offset: 4
// End:
//
// vim: ts=4 sts=4 sw=4 expandtab foldmethod=marker
//
