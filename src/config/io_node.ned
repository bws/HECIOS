//
// This file is part of Hecios
//
// Copyright (C) 2007,2008,2009 Brad Settlemyer
//
// This file is distributed WITHOUT ANY WARRANTY. See the file 'License.txt'
// for details on this and other legal matters.
//

import bmi_enhanced_host;
import daemon_process;
import Storagelayer;

//
// Ethernet based compute node
//
module IONode

{
    parameters:
        @display("bgb=,,white,,");

    gates:
        input ethIn;
        output ethOut;

    submodules:
        hca: EnhancedHost {
            parameters:
                numBmiApps = 1;
                bmiAppType = "BMITcpServer";
                @display("p=100,80;i=block/rxtx,medium purple");

        }
        daemon: DaemonProcess {
            parameters:
                @display("p=100,180;i=block/process,blue");

        }
        os: OperatingSystem {
            parameters:
                @display("p=100,280;i=block/process,blue");

        }
        hardDisk: BasicModelDisk {
            parameters:
                @display("p=100,380;i=abstract/db,sienna");

        }
    connections:

        // Connect process to bmi transport
        daemon.out --> hca.bmig$i++;
        daemon.in <-- hca.bmig$o++;

        // Connect operating system gates
        daemon.toOS --> os.in;
        daemon.fromOS <-- os.out;

        // Connect storage gates
        os.toDisk --> hardDisk.in;
        os.fromDisk <-- hardDisk.out;

        // Connect networking gates
        ethIn --> hca.ethg$i++;
        ethOut <-- hca.ethg$o++;

}

