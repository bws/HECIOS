
import
    "compute_node",
    "io_node",
    "switch",
     "Network/AutoRouting/FlatNetworkConfigurator";

//
// Beowulf commodity cluster model
//
module Cluster
    parameters:
        num_cpu_nodes : const,
        num_io_nodes : const;

    submodules:
        cpun: ComputeNode[num_cpu_nodes]
            display: "i=block/process,cyan"; // "i=<icon>,<color>"

        ion: IONode[num_io_nodes]
            display: "i=abstract/server,green";

        switch1: Switch
            parameters:
                num_ports = sizeof(ion) + sizeof(cpun);
            gatesizes:
                in[sizeof(ion) + sizeof(cpun)], 
                out[sizeof(ion) + sizeof(cpun)];
            display: "i=device/switch,pink";
    
    connections:
        for i = 0..num_cpu_nodes-1 do
            cpun[i].out --> delay 10ms --> switch1.in++;
            cpun[i].in <-- delay 10ms <-- switch1.out++;
        endfor;

        for i = 0..num_io_nodes-1 do
            ion[i].out --> delay 10ms --> switch1.in++;
            ion[i].in <-- delay 10ms <-- switch1.out++;
        endfor;

        //for i=0..num_cpu_nodes-1, j=0..num_io_nodes-1 do
        //    cpun[i].out --> delay 10ms --> ion[j].in;
        //    ion[j].out --> delay 100ms --> cpun[i].in;
        //endfor;
endmodule

//
// Network for a Beowulf commodity cluster
//
network cluster1 : Cluster
endnetwork

