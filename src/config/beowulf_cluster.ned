
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

        fsConfig: FSServerConfigurator;
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


// Settings for a beowulf cluster similar to adenine
network adenine: BeowulfCluster

endnetwork