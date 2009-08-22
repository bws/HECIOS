//
// This file is part of Hecios
//
// Copyright (C) 2007,2008,2009 Brad Settlemyer
//
// This file is distributed WITHOUT ANY WARRANTY. See the file 'License.txt'
// for details on this and other legal matters.
//

//
// Ethernet based compute node
//
module ComputeNode
{
    parameters:
        double numProcs;

    gates:
        input ethIn;
        output ethOut;

    submodules:
        process[numProcs]: JobProcess {
            parameters:
                @display("p=100,180,m,4;i=block/process,blue");

        }
        hca: EnhancedHost {
            parameters:
                numBmiApps = numProcs;
                numMpiApps = numProcs;
                bmiAppType = "BMITcpClient";
                mpiAppType = "MPITcpServer";
                @display("p=100,80;i=block/rxtx,medium purple");

        }
    connections:
        // Connect processes to BMI network transport
        for i=0..numProcs-1 {
            process[i].bmiOut --> hca.bmig$i++;
            process[i].bmiIn <-- hca.bmig$o++;
            process[i].mpiOut --> hca.mpig$i++;
            process[i].mpiIn <-- hca.mpig$o++;
        }

        // Connect forwarding gates
        ethIn --> hca.ethg$i++;
        ethOut <-- hca.ethg$o++;

}

