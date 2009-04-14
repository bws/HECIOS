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

    gates:
        in: bmiIn, mpiIn;
        out: bmiOut, mpiOut;

    submodules:
        mpi: MPIProcess;
            display: "p=120,80;i=block/process,white";
           
        clientJobManager: JobManager;
            display: "p=60, 280;i=block/classifier,yellow";

        fsClient: FSClient;
            display: "p=160,280;i=block/process,red";

    connections nocheck:
        // Connect the MPI process I/O handling to the cache
        mpi.ioOut --> fsClient.appIn;
        mpi.ioIn <-- fsClient.appOut;

        // Connect the file system network to job manager pfs ports
        fsClient.netOut --> clientJobManager.pfsIn;
        fsClient.netIn <-- clientJobManager.pfsOut;

        // Connect the client job manager's network ports to the network
        clientJobManager.netOut --> bmiOut;
        clientJobManager.netIn <-- bmiIn;
        
        // Connect the MPI process's network ports to the network
        mpi.netOut --> mpiOut;
        mpi.netIn <-- mpiIn;

    display: "o=white";
endmodule
