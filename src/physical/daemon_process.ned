
import
    "bmi_tcp_server",
    "fs_server";

//
// A UNIX Daemon process
//
module DaemonProcess

    gates:
        in: in;
        out: out;

    submodules:
        server: FSServer;
           
    connections nocheck:
        server.netOut --> out;
        server.netIn <-- in; 

endmodule
