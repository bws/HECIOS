
import
    "job_process", 
    "bmi_enhanced_host";

//
// Ethernet based compute node
//
module ComputeNode

    gates:
        in: ethIn;
        out: ethOut;

    submodules:
        jobProcess: JobProcess;

        // Note:  Statically, we are only able to construct TCPApps of a
        // single type, however the underlying TCPApp's will be
        // changed dynamically to be of the types:
        // 0.  BMITcpClient
        // 1.  MPITcpClient
        // 2.  MPITcpServer
        hca: EnhancedHost;
        parameters:
            numTcpApps = 3,
            tcpAppType = "BMITcpClient";

    connections:

        // Connect process to network transport
        for i=0..2 do
            jobProcess.out++ --> hca.bmiIn++;
            jobProcess.in++ <-- hca.bmiOut++;
        endfor

        // Connect forwarding gates
        ethIn --> hca.ethIn++;
        ethOut <-- hca.ethOut++;

endmodule
