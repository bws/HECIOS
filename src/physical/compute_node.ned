
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

        // Connect process to bmi transport
        //jobProcess.out --> hca.tcpApp.msgIn;
        //jobProcess.in <-- hca.tcpApp.msgOut;

        // Connect forwarding gates
        ethIn --> hca.ethIn++;
        ethOut <-- hca.ethOut++;

endmodule