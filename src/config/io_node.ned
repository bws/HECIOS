//
// This file is part of Hecios
//
// Copyright (C) 2007 Brad Settlemyer
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
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
