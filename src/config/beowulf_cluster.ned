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
    "compute_node",
    "io_node",
    "fs_server_configurator",
    "mpi_configurator",
    "EtherSwitch",
    "FlatNetworkConfigurator";

module BeowulfCluster

    parameters:
        numCPUNodes: const,
        numIONodes: const;

    submodules:
        cpun: ComputeNode[numCPUNodes];
            display: "p=20,150,m,8;i=abstract/server,yellow";

        ion: IONode[numIONodes];
            display: "p=20,380,m,8;i=abstract/cache,magenta";

        switch: EtherSwitch;
            display: "p=256,250;i=abstract/switch,cyan";

        pfsConfig: FSServerConfigurator;
            parameters:
                handlesPerServer = 10000;
            display: "p=40,50;i=abstract/table2,light blue";

        mpiConfig: MPIConfigurator;
            display: "p=140,50;i=abstract/table2,khaki";

        networkConfig: FlatNetworkConfigurator;
            parameters:
                moduleTypes = "Router StandardHost BMIEnhancedHost EnhancedHost",
                nonIPModuleTypes = "",
                networkAddress = "192.168.0.0",
                netmask = "255.255.0.0";
            display: "p=240,50;i=abstract/table2,hot pink";

    connections:

        for i = 0..numCPUNodes-1 do
            cpun[i].ethOut --> switch.in++;
            cpun[i].ethIn <-- switch.out++;
        endfor;

        for i = 0..numIONodes - 1 do
            ion[i].ethOut --> switch.in++;
            ion[i].ethIn <-- switch.out++;
        endfor;

    display: "o=white"; // affects background

endmodule


// Network for testing and general simulator use
network Test: BeowulfCluster
endnetwork

// Settings for a beowulf cluster similar to adenine
network adenine: BeowulfCluster
endnetwork


//
// Local variables:
//  indent-tabs-mode: nil
//  c-indent-level: 4
//  c-basic-offset: 4
// End:
//
// vim: ts=4 sts=4 sw=4 expandtab
//
