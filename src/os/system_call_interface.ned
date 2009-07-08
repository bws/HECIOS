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
// Implemented System Call Interfaces:
// -  PassThroughSystemCallInterface
//
simple SystemCallInterface
  parameters:
      addReadOverheadSecs: const,
      addWriteOverheadSecs: const,
      overheadSecs: const;

  gates:
    in: in, response;
    out: out, request;
endsimple


