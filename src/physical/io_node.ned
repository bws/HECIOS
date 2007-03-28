
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
        daemon: DaemonProcess;

        hca: StandardHost;
            parameters:
                numTcpApps = 1,
                tcpAppType = "BMITcpServer";

        //hdd: Disk;

    connections nocheck:

        ethIn --> hca.ethIn++;
        ethOut <-- hca.ethOut++;

endmodule