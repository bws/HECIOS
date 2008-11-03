//
// This file is part of Hecios
//
// Copyright (C) 2006 Joel Sherrill <joel@oarcorp.com>
// Copyright (C) 2007 Brad Settlemyer
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
//

//
// Template for OS Block device buffer caches
//
simple BufferCache
    gates:
        in: in, response;
        out: out, request;
endsimple

//
// Passthrough/No-op buffer cache
//
simple NoBufferCache
    gates:
        in: in, response;
        out: out, request;
endsimple

//
// BufferCache using an LRU replacement policy
//
simple LRUBufferCache
    parameters:
        numEntries : const,
        dirtyThreshold: const;

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
