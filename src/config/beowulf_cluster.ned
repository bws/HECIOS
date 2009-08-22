import compute_node;
import io_node;
import fs_server_configurator;
import mpi_configurator;
import EtherSwitch;
import inet.networklayer.autorouting.FlatNetworkConfigurator;

// Data channel operating at 100Mbs
channel FastE extends ned.DatarateChannel
{
    parameters:
        delay = 0.0;   // sec
        ber = 0.0;
        datarate = 100.0Mbps;   // bit/sec
}

// Data channel operating at 1Gbs
channel GigE extends ned.DatarateChannel
{
    parameters:
        delay = 0.0;   // sec
        ber = 0.0;
        datarate = 1.0Gbps; // bit/sec
}

// Data channel operating at 10Gbs
channel TenGigE extends ned.DatarateChannel
{
    parameters:
        delay = 0.0;   // sec
        ber = 0.0;
        datarate = 10.0Gbps; // bit/sec
}

// Data channel operating at 40Gbs
channel FortyGigE extends ned.DatarateChannel
{
    parameters:
        delay = 0.0;   // sec
        ber = 0.0;
        datarate = 40.0Gbps; // bit/sec
}

// Data channel representing Myrinet 10g performance
channel Myrinet10G extends ned.DatarateChannel
{
    parameters:
        delay = 0.0;
        ber = 0.0;
        datarate = 10.0Gbps;
}

module BeowulfClusterFastE

{
    parameters:
        double numCPUNodes;
        double numIONodes;

        @display("bgb=,,white,,"); // affects background
    submodules:
        cpun[numCPUNodes]: ComputeNode {
            parameters:
                @display("p=40,120,m,4;i=abstract/server,yellow");

        }
        ion[numIONodes]: IONode {
            parameters:
                @display("p=40,380,m,4;i=abstract/cache,magenta");

        }
        switch: EnhancedEtherSwitch2 {
            parameters:
                @display("p=140,290;i=abstract/switch,cyan");

        }
        pfsConfig: FSServerConfigurator {
            parameters:
                handlesPerServer = 250000;
                @display("p=40,50;i=abstract/table2,light blue");

        }
        mpiConfig: MPIConfigurator {
            parameters:
                @display("p=140,50;i=abstract/table2,khaki");

        }
        networkConfig: FlatNetworkConfigurator {
            parameters:
                networkAddress = "192.168.0.0";
                netmask = "255.255.0.0";
                @display("p=240,50;i=abstract/table2,hot pink");

        }
    connections:

        for i=0..numCPUNodes-1 {
            cpun[i].ethOut --> FastE --> switch.ethg$i++;
            cpun[i].ethIn <-- FastE <-- switch.ethg$o++;
        }

        for i=0..numIONodes-1 {
            ion[i].ethOut --> FastE --> switch.ethg$i++;
            ion[i].ethIn <-- FastE <-- switch.ethg$o++;
        }
}

module BeowulfClusterGigE

{
    parameters:
        double numCPUNodes;
        double numIONodes;

        @display("bgb=,,white,,"); // affects background
    submodules:
        cpun[numCPUNodes]: ComputeNode {
            parameters:
                @display("p=40,120,m,4;i=abstract/server,yellow");

        }
        ion[numIONodes]: IONode {
            parameters:
                @display("p=40,380,m,2;i=abstract/cache,magenta");

        }
        switch: EnhancedEtherSwitch2 {
            parameters:
                @display("p=140,290;i=abstract/switch,cyan");

        }
        pfsConfig: FSServerConfigurator {
            parameters:
                handlesPerServer = 250000;
                @display("p=40,50;i=abstract/table2,light blue");

        }
        mpiConfig: MPIConfigurator {
            parameters:
                @display("p=140,50;i=abstract/table2,khaki");

        }
        networkConfig: FlatNetworkConfigurator {
            parameters:
                networkAddress = "192.168.0.0";
                netmask = "255.255.0.0";
                @display("p=240,50;i=abstract/table2,hot pink");

        }
    connections:

        for i=0..numCPUNodes-1 {
            cpun[i].ethOut --> GigE --> switch.ethg$i++;
            cpun[i].ethIn <-- GigE <-- switch.ethg$o++;
        }

        for i=0..numIONodes-1 {
            ion[i].ethOut --> GigE --> switch.ethg$i++;
            ion[i].ethIn <-- GigE <-- switch.ethg$o++;
        }
}

module BeowulfClusterMyri10G

{
    parameters:
        double numCPUNodes;
        double numIONodes;

        @display("bgb=,,white,,"); // affects background
    submodules:
        cpun[numCPUNodes]: ComputeNode {
            parameters:
                @display("p=40,120,m,4;i=abstract/server,yellow");

        }
        ion[numIONodes]: IONode {
            parameters:
                @display("p=40,380,m,2;i=abstract/cache,magenta");

        }
        switch: EnhancedEtherSwitch2 {
            parameters:
                @display("p=140,290;i=abstract/switch,cyan");

        }
        pfsConfig: FSServerConfigurator {
            parameters:
                handlesPerServer = 250000;
                @display("p=40,50;i=abstract/table2,light blue");

        }
        mpiConfig: MPIConfigurator {
            parameters:
                @display("p=140,50;i=abstract/table2,khaki");

        }
        networkConfig: FlatNetworkConfigurator {
            parameters:
                networkAddress = "192.168.0.0";
                netmask = "255.255.0.0";
                @display("p=240,50;i=abstract/table2,hot pink");

        }
    connections:

        for i=0..numCPUNodes-1 {
            cpun[i].ethOut --> Myrinet10G --> switch.ethg$i++;
            cpun[i].ethIn <-- Myrinet10G <-- switch.ethg$o++;
        }

        for i=0..numIONodes-1 {
            ion[i].ethOut --> Myrinet10G --> switch.ethg$i++;
            ion[i].ethIn <-- Myrinet10G <-- switch.ethg$o++;
        }
}


// Network for testing and general simulator use
network Test extends BeowulfClusterFastE
{
    parameters:
}

// Settings for a beowulf cluster similar to adenine
network adenine extends BeowulfClusterFastE
{
    parameters:
}

// Settings for a beowulf cluster similar to Jazz
network Jazz extends BeowulfClusterFastE
{
    parameters:
}

// Settings for a beowulf cluster similar to Palmetto
network PalmettoGigE extends BeowulfClusterGigE
{
    parameters:
}

// Settings for a beowulf cluster similar to Palmetto
network PalmettoMyri10G extends BeowulfClusterMyri10G
{
    parameters:
}


