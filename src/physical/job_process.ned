
import
    "bmi_transport",
    "mpi_process";

//
// A UNIX process
//
module JobProcess

    gates:
        in: in;
        out: out;

    submodules:
        mpi: MPIProcess;
           
    connections:
        mpi.netOut --> out;
        mpi.netIn <-- in;

endmodule
