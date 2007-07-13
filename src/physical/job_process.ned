
import
    "bmi_transport",
    "mpi_process";

//
// A UNIX process that requires 10us interrupts to access the network
//
module JobProcess

    gates:
        in: in;
        out: out;

    submodules:
        mpi: MPIProcess;
           
    connections:
        mpi.netOut --> delay 10us -->out;
        mpi.netIn <-- delay 10us <-- in;

endmodule
