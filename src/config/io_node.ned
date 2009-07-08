//
// This file is part of Hecios
//
// Copyright (C) 2007,2008,2009 Brad Settlemyer
//
// This file is distributed WITHOUT ANY WARRANTY. See the file 'License.txt'
// for details on this and other legal matters.
//

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

        os: OperatingSystem;
            display: "p=100,280;i=block/process,blue";

        hardDisk: BasicModelDisk;
            display: "p=100,380;i=abstract/db,sienna";

    connections:

        // Connect process to bmi transport
        daemon.out --> hca.bmiIn++;
        daemon.in <-- hca.bmiOut++;

        // Connect operating system gates
        daemon.toOS --> os.in;
        daemon.fromOS <-- os.out;

        // Connect storage gates
        os.toDisk --> hardDisk.in;
        os.fromDisk <-- hardDisk.out;

        // Connect networking gates
        ethIn --> hca.ethIn++;
        ethOut <-- hca.ethOut++;

    display: "o=white";
        
endmodule

//
// Local variables:
//  indent-tabs-mode: nil
//  c-indent-level: 4
//  c-basic-offset: 4
// End:
//
// vim: ts=4 sts=4 sw=4 expandtab
//
