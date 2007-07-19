
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
            display: "p=20,120,m,8;i=device/cpu,yellow";

        ion: IONode[numIONodes];
            display: "p=20,300,m,8;i=abstract/server,magenta";

        switch: EtherSwitch;
            display: "p=256,200;i=abstract/switch,cyan";

        fsConfig: FSServerConfigurator;
            parameters:
                handlesPerServer = 10000;
            display: "p=40,50;b=20,20,rect;o=blue";

        mpiConfig: MPIConfigurator;
            display: "p=140,50;b=20,20,rect;o=blue";

        networkConfig: FlatNetworkConfigurator;
            parameters:
                moduleTypes = "Router StandardHost BMIEnhancedHost EnhancedHost",
                nonIPModuleTypes = "",
                networkAddress = "192.168.0.0",
                netmask = "255.255.0.0";
            display: "p=240,50;b=20,20,rect;o=blue";

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