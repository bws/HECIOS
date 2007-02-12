
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

        server.out --> hca.in++;
        server.in <-- hca.out++;

        // Connect forwarding gates
        ethIn --> hca.ethIn++;
        ethOut <-- hca.ethOut++;
            
endmodule