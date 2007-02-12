
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
        mpi: MPIProcess;
           
        //bmi: BMITransport;

    connections:
        mpi.extOut --> out;
        mpi.extIn <-- in; 
endmodule
