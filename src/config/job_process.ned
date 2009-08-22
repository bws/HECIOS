//
// This file is part of Hecios
//
// Copyright (C) 2007,2008,2009 Brad Settlemyer
//
// This file is distributed WITHOUT ANY WARRANTY. See the file 'License.txt'
// for details on this and other legal matters.
//

import bmi_transport;
import mpi_process;

//
// A UNIX process that requires 10us interrupts to access the network
//
module JobProcess
{
    parameters:

        @display("bgb=,,white,,");
    gates:
        input bmiIn;
        input mpiIn;
        output bmiOut;
        output mpiOut;

    submodules:
        mpi: MPIProcess {
            parameters:
                @display("p=120,80;i=block/process,white");

        }
        clientJobManager: JobManager {
            parameters:
                @display("p=60, 280;i=block/classifier,yellow");

        }
        fsClient: FSClient {
            parameters:
                @display("p=160,280;i=block/process,red");

        }
    connections allowunconnected:
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

}
