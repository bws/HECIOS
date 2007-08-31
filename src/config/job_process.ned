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

    gates:
        in: in[];
        out: out[];

    submodules:
        mpi: MPIProcess;
           
    connections:

        // Connect the MPI process I/O network to this Job's network
        mpi.ioOut --> delay 10us --> out++;
        mpi.ioIn <-- delay 10us <-- in++;

        // Connect the MPI process messaging network to this Job's network
        // (The connection order here is important)
        mpi.mpiClientOut --> out++;
        mpi.mpiClientIn <-- in++;
        mpi.mpiServerOut --> out++;
        mpi.mpiServerIn <-- in++;

endmodule
