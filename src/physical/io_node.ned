
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

        hca: BMIEnhancedHost;
            parameters:
                numTcpApps = 1,
                tcpAppType = "BMITcpServer";

        storage: StorageLayer;

    connections nocheck:

        // Connect process to bmi transport
        daemon.out --> hca.bmiIn++;
        daemon.in <-- hca.bmiOut++;

        // Connect storage gates
        daemon.storageOut --> storage.in;
        daemon.storageIn <-- storage.out;

        // Connect networking gates
        ethIn --> hca.ethIn++;
        ethOut <-- hca.ethOut++;

endmodule