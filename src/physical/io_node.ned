
import
    "bmi_enhanced_host",
    "daemon_process", 
    "Storagelayer";

//
// Ethernet based compute node
//
module IONode

    gates:
        in: ethIn;
        out: ethOut;

    submodules:
        hca: BMIEnhancedHost;
            parameters:
                numTcpApps = 1,
                tcpAppType = "BMITcpServer";
            display: "p=100,80;i=block/rxtx,medium purple";

        daemon: DaemonProcess;
            display: "p=100,180;i=block/process,blue";

        storage: StorageLayer;
            display: "p=100,280;i=abstract/db,sienna";

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

    display: "o=white";
        
endmodule