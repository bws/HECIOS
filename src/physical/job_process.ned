
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
           
        //bmi: BMITransport;
 
    connections:
        mpi.netOut --> out;
        mpi.netIn <-- in;

endmodule
