//
// This file is part of Hecios
//
// Copyright (C) 2007,2008,2009 Brad Settlemyer
//
// This file is distributed WITHOUT ANY WARRANTY. See the file 'License.txt'
// for details on this and other legal matters.
//
import
    "compute_node",
    "io_node",
    "fs_server_configurator",
    "mpi_configurator",
    "EtherSwitch",
    "FlatNetworkConfigurator";

// Data channel operating at 100Mbs
channel FastE
    delay 0.0   // sec
    error 0.0
    datarate 100000000.0   // bit/sec
endchannel

// Data channel operating at 1Gbs
channel GigE
    delay 0.0   // sec
    error 0.0
    datarate 1000000000.0 // bit/sec
endchannel

// Data channel operating at 10Gbs
channel TenGigE
    delay 0.0   // sec
    error 0.0
    datarate 10000000000.0 // bit/sec
endchannel

// Data channel operating at 40Gbs
channel FortyGigE
    delay 0.0   // sec
    error 0.0
    datarate 40000000000.0 // bit/sec
endchannel

// Data channel representing Myrinet 10g performance
channel Myrinet10G
    delay 0.0
    error 0.0
    datarate 10000000000.0
endchannel

module BeowulfClusterFastE

    parameters:
        numCPUNodes: const,
        numIONodes: const;

    submodules:
        cpun: ComputeNode[numCPUNodes];
            display: "p=40,120,m,4;i=abstract/server,yellow";

        ion: IONode[numIONodes];
            display: "p=40,380,m,4;i=abstract/cache,magenta";

        switch: EnhancedEtherSwitch2;
            display: "p=140,290;i=abstract/switch,cyan";

        pfsConfig: FSServerConfigurator;
            parameters:
                handlesPerServer = 250000;
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
			cpun[i].ethOut --> FastE --> switch.in++;
            cpun[i].ethIn <-- FastE <-- switch.out++;
        endfor;

        for i = 0..numIONodes - 1 do
			ion[i].ethOut --> FastE --> switch.in++;
            ion[i].ethIn <-- FastE <-- switch.out++;
        endfor;
    display: "o=white"; // affects background
endmodule

module BeowulfClusterGigE

    parameters:
        numCPUNodes: const,
        numIONodes: const;

    submodules:
        cpun: ComputeNode[numCPUNodes];
            display: "p=40,120,m,4;i=abstract/server,yellow";

        ion: IONode[numIONodes];
            display: "p=40,380,m,2;i=abstract/cache,magenta";

        switch: EnhancedEtherSwitch2;
            display: "p=140,290;i=abstract/switch,cyan";

        pfsConfig: FSServerConfigurator;
            parameters:
                handlesPerServer = 250000;
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
            cpun[i].ethOut --> GigE --> switch.in++;
            cpun[i].ethIn <-- GigE <-- switch.out++;
        endfor;

        for i = 0..numIONodes - 1 do
			ion[i].ethOut --> GigE --> switch.in++;
            ion[i].ethIn <-- GigE <-- switch.out++;
        endfor;
    display: "o=white"; // affects background
endmodule

module BeowulfClusterMyri10G

    parameters:
        numCPUNodes: const,
        numIONodes: const;

    submodules:
        cpun: ComputeNode[numCPUNodes];
            display: "p=40,120,m,4;i=abstract/server,yellow";

        ion: IONode[numIONodes];
            display: "p=40,380,m,2;i=abstract/cache,magenta";

        switch: EnhancedEtherSwitch2;
            display: "p=140,290;i=abstract/switch,cyan";

        pfsConfig: FSServerConfigurator;
            parameters:
                handlesPerServer = 250000;
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
            cpun[i].ethOut --> Myrinet10G --> switch.in++;
            cpun[i].ethIn <-- Myrinet10G <-- switch.out++;
        endfor;

        for i = 0..numIONodes - 1 do
            ion[i].ethOut --> Myrinet10G --> switch.in++;
            ion[i].ethIn <-- Myrinet10G <-- switch.out++;
        endfor;
    display: "o=white"; // affects background
endmodule


// Network for testing and general simulator use
network Test: BeowulfClusterFastE
endnetwork

// Settings for a beowulf cluster similar to adenine
network adenine: BeowulfClusterFastE
endnetwork

// Settings for a beowulf cluster similar to Jazz
network Jazz: BeowulfClusterFastE
endnetwork

// Settings for a beowulf cluster similar to Palmetto
network PalmettoGigE: BeowulfClusterGigE
endnetwork

// Settings for a beowulf cluster similar to Palmetto
network PalmettoMyri10G: BeowulfClusterMyri10G
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
