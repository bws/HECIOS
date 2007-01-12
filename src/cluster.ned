
import
    "compute_node",
    "io_node",
    "switch",
    "Network/AutoRouting/FlatNetworkConfigurator",
    "StandardHost",
    "Router";

channel gig_link
    datarate 1000000000
    delay 10ms
endchannel

//
// Beowulf commodity cluster model
//
module Cluster
    parameters:
        num_cpu_nodes : const,
        num_io_nodes : const;

    submodules:
        configurator: FlatNetworkConfigurator;
            parameters:
                moduleTypes = "Router StandardHost",
                nonIPModuleTypes = "",
                networkAddress = "192.168.0.0",
                netmask = "255.255.0.0";

        cpun: StandardHost[num_cpu_nodes];
            display: "p=56,64;i=block/process,cyan";

        ion: StandardHost[num_io_nodes];
            display: "p=42,144;i=abstract/server,green";

        router: Router;
            display: "p=147,143;i=abstract/router";
    
    connections:

        for i = 0..num_cpu_nodes-1 do
            cpun[i].out++ --> gig_link --> router.in++;
            cpun[i].in++ <-- gig_link <-- router.out++;
        endfor;

        for i = 0..num_io_nodes-1 do
            ion[i].out++ --> gig_link --> router.in++;
            ion[i].in++ <-- gig_link <-- router.out++;
        endfor;

endmodule

//
// Network for a Beowulf commodity cluster
//
network beowulf_net : Cluster
endnetwork

