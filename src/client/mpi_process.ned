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

import
    "io_application",
    "cache_module",
    "fs_client",
    "bmi_transport",
    "mpi_middleware",
    "cacheModule";

//
// Compound module describing and MPI job process
//
module MPIProcess
    gates:
        in: ioIn, mpiClientIn, mpiServerIn;
        out: ioOut, mpiClientOut, mpiServerOut;

    submodules:
        app: IOApplication;
            display: "p=130,80;i=block/app,white";

	mpiMiddleware: MpiMiddleware;
	    display: "p=180,180;i=block/classifier,red";

    connections:
        // Connect the MPI application to the MPI middleware
	app.mpiOut --> mpiMiddleware.appIn;
	app.mpiIn <-- mpiMiddleware.appOut;

        // Connect the MPI application to the I/O middleware
        app.ioOut --> ioOut;
        app.ioIn <-- ioIn;

	// Connect the MPI middleware to the MPI messaging network
        mpiMiddleware.netClientOut --> mpiClientOut;
        mpiMiddleware.netClientIn <-- mpiClientIn;
        mpiMiddleware.netServerOut --> mpiServerOut;
        mpiMiddleware.netServerIn <-- mpiServerIn;

    display: "o=white";
endmodule
