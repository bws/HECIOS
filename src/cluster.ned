
import
    "compute_node",
    "io_node",
    "EtherSwitch",
    "EtherSwitch2",
    "FlatNetworkConfigurator",
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

        switch: EtherSwitch;
            display: "p=147,143;i=abstract/switch";
    
    connections:

        for i = 0..num_cpu_nodes-1 do
            cpun[i].ethOut++ --> switch.in++;
            cpun[i].ethIn++ <-- switch.out++;
        endfor;

        for i = 0..num_io_nodes-1 do
            ion[i].ethOut++ --> switch.in++;
            ion[i].ethIn++ <-- switch.out++;
        endfor;

endmodule

network test_cluster: Cluster
    parameters:
        num_cpu_nodes = 4,
        num_io_nodes = 4;
//        switch.relayUnitType = "MACRelayUnitPP";
//        switch.relayUnit.addressTableFile = "",
//        switch.relayUnit.addressTableSize = 100000,
//        switch.mac[*].address = "auto",
//        switch.mac[*].txrate = 136000000000,
//        switch.mac[*].duplexEnabled = true,
//        switch.mac[*].writeScalars = false;

endnetwork