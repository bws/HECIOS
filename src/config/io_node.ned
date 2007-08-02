
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

        hardDisk: BasicModelDisk;
            display: "p=100,280;i=abstract/db,sienna";

    connections nocheck:

        // Connect process to bmi transport
        daemon.out --> hca.bmiIn++;
        daemon.in <-- hca.bmiOut++;

        // Connect storage gates
        daemon.toDisk --> hardDisk.in;
        daemon.fromDisk <-- hardDisk.out;

        // Connect networking gates
        ethIn --> hca.ethIn++;
        ethOut <-- hca.ethOut++;

    display: "o=white";
        
endmodule

//
// Local variables:
//  c-indent-level: 4
//  c-basic-offset: 4
// End:
//
// vim: ts=4 sts=4 sw=4 expandtab foldmethod=marker
//
