
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
        parameters:
            numTcpApps = 1,
            tcpAppType = "BMITransport";

    connections nocheck:

        // Connect forwarding gates
        jobProcess.out --> hca.ethIn++;
        jobProcess.in <-- hca.ethOut++;

endmodule