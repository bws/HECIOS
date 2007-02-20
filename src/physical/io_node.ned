
import
    "daemon_process", 
    "StandardHost";

//
// Ethernet based compute node
//
module IONode

    gates:
        in: ethIn;
        out: ethOut;

    submodules:
        server: DaemonProcess;

        hca: StandardHost;

        //hdd: Disk;

    connections:

        ethIn --> hca.ethIn++;
        ethOut <-- hca.ethOut++;

        server.out --> hca.ethIn++;
        server.in <-- hca.ethOut++;
endmodule