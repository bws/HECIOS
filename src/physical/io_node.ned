
import
    "daemon_process", 
    "StandardHost",
    "Storagelayer";

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

        storage: StorageLayer;

    connections nocheck:

        ethIn --> hca.ethIn++;
        ethOut <-- hca.ethOut++;

        daemon.storageOut --> storage.in;
        daemon.storageIn <-- storage.out;

endmodule