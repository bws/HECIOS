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
    "bmi_transport",
    "mpi_process";

//
// A UNIX process that requires 10us interrupts to access the network
//
module JobProcess
    parameters:
        middlewareCacheType : string;

    gates:
        in: in[];
        out: out[];

    submodules:
        mpi: MPIProcess;
            display: "p=120,80;i=block/process,white";
           
        cache: middlewareCacheType like MiddlewareCache;
            display: "p=120,180;i=block/buffer2,green";

        clientJobManager: JobManager;
            display: "p=60, 280;i=block/classifier,yellow";

        fsClient: FSClient;
            display: "p=160,280;i=block/process,red";

    connections nocheck:
        // Connect the MPI process I/O handling to the cache
        mpi.ioOut --> cache.appIn;
        mpi.ioIn <-- cache.appOut;

        // Connect the cache to the file system client
        cache.fsOut --> fsClient.appIn;
        cache.fsIn <-- fsClient.appOut;

        // Connect the file system network to job manager pfs ports
        fsClient.netOut --> clientJobManager.pfsIn;
        fsClient.netIn <-- clientJobManager.pfsOut;

        // Connect the client job manager's network ports to the network
        clientJobManager.netOut -->out++;
        clientJobManager.netIn <-- in++;

        // Connect the MPI process messaging network to the network
        // (The connection order here is important)
        mpi.mpiClientOut --> out++;
        mpi.mpiClientIn <-- in++;
        mpi.mpiServerOut --> out++;
        mpi.mpiServerIn <-- in++;

    display: "o=white";
endmodule
