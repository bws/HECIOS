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
    parameters:
        numProcs: numeric const;

    gates:
        in: ethIn;
        out: ethOut;

    submodules:
        process: JobProcess[numProcs];
            display: "p=100,180,m,4;i=block/process,blue";

        hca: EnhancedHost;
        parameters:
            numBMIApps = numProcs,
            numMPIApps = numProcs,
            bmiAppType = "BMITcpClient",
            mpiAppType = "MPITcpServer";
        display: "p=100,80;i=block/rxtx,medium purple";

    connections:
        // Connect processes to BMI network transport
        for i=0..numProcs-1 do
            process[i].bmiOut --> hca.bmiIn++;
            process[i].bmiIn <-- hca.bmiOut++;
            process[i].mpiOut --> hca.mpiIn++;
            process[i].mpiIn <-- hca.mpiOut++;
        endfor

        // Connect forwarding gates
        ethIn --> hca.ethIn++;
        ethOut <-- hca.ethOut++;

endmodule

//
// Local variables:
//  indent-tabs-mode: nil
//  c-indent-level: 4
//  c-basic-offset: 4
// End:
//
// vim: ts=4 sts=4 sw=4 expandtab
//
