
import
    "compute_node",
    "io_node",
    "EtherSwitch",
    "FlatNetworkConfigurator";

module BeowulfCluster

    parameters:
        numCPUNodes: const,
        numIONodes: const;

    submodules:

        cpun: ComputeNode[numCPUNodes];
            display: "p=20,32,r;i=block/process,cyan";

        ion: IONode[numIONodes];
            display: "p=20,256,r;i=abstract/server,green";

        switch: EtherSwitch;
            display: "p=256,164;i=abstract/switch";

        configurator: FlatNetworkConfigurator;
            parameters:
                moduleTypes = "Router StandardHost",
                nonIPModuleTypes = "",
                networkAddress = "192.168.0.0",
                netmask = "255.255.0.0";

    connections:

        for i = 0..numCPUNodes-1 do
            cpun[i].ethOut --> switch.in++;
            cpun[i].ethIn <-- switch.out++;
        endfor;

        for i = 0..numIONodes - 1 do
            ion[i].ethOut --> switch.in++;
            ion[i].ethIn <-- switch.out++;
        endfor;

endmodule


// Settings for a beowulf cluster similar to adenine
network adenine: BeowulfCluster

endnetwork