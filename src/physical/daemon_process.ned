
import
    "bmi_tcp_server",
    "fs_server";

//
// A UNIX Daemon process
//
module DaemonProcess

    gates:
        in: in, storageIn;
        out: out, storageOut;

    submodules:
        server: FSServer;
           
    connections nocheck:
        server.netOut --> out;
        server.netIn <-- in; 

        server.storageOut --> storageOut;
        server.storageIn <-- storageIn;

endmodule
