
import
    "bmi_transport",
    "mpi_process";

//
// A UNIX Daemon process
//
module DaemonProcess

    gates:
        in: in;
        out: out;

    submodules:
        //mpi: MPIProcess;
           
    connections nocheck:
        //mpi.netOut --> out;
        //mpi.netIn <-- in; 

endmodule
