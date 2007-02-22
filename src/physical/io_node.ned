
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

    connections nocheck:

        ethIn --> hca.ethIn++;
        ethOut <-- hca.ethOut++;

endmodule