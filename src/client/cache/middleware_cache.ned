//
// This file is part of Hecios
//
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
// Template for middleware MPI-IO process caching
//
simple MiddlewareCache
    gates:
        in: appIn, fsIn;
        out: appOut, fsOut;
endsimple

//
// Passthrough/No-op middleware cache
//
simple NoMiddlewareCache
    gates:
        in: appIn, fsIn;
        out: appOut, fsOut;
endsimple

//
// Middleware cache using pages of direct cache
//
simple DirectPagedMiddlewareCache
    parameters:
    	pageSize: const,
    	pageCapacity: const;
    	
	gates:
		in: appIn, fsIn;
        out: appOut, fsOut;
endsimple

//
// Middleware cache using pages of direct cache
//
simple SharedDirectMiddlewareCache
    parameters:
    	pageSize: const,
    	pageCapacity: const;
    	
	gates:
		in: appIn, fsIn;
        out: appOut, fsOut;
endsimple

//
// Middleware cache using pages of fully associative cache
//
simple FullyPagedMiddlewareCache
    parameters:
        pageSize: const,
        pageCapacity: const;
        
    gates:
        in: appIn, fsIn;
        out: appOut, fsOut;
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