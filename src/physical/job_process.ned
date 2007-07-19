
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
