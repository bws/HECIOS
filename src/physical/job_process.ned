
import
    "bmi_transport",
    "mpi_process",
    "StandardHost";

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
        mpi.extOut --> out;
        mpi.extIn <-- in;

endmodule
