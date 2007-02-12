
import
    "job_process", 
    "StandardHost";

//
// Ethernet based compute node
//
module ComputeNode

    gates:
        in: ethIn;
        out: ethOut;

    submodules:
        jobProcess: JobProcess;

        hca: StandardHost;

    connections:

        // Connect forwarding gates
        jobProcess.out --> hca.in++;
        jobProcess.in <-- hca.out++;       

        // Connect forwarding gates
        ethIn --> hca.ethIn++;
        ethOut <-- hca.ethOut++;

endmodule