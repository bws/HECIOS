
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

        hca: BMIEnhancedHost;
        parameters:
            numTcpApps = 1,
            tcpAppType = "BMITcpClient";

        //mpiClientHCA: EnhancedHost;
        //parameters:
        //    numTcpApps = 1,
        //    tcpAppType ="MPITCPClient";

        //mpiServerHCA: EnhancedHost;
        //parameters:
        //    numTcpApps = 1,
        //    tcpAppType ="MPITCPServer";

    connections:

        // Connect process to bmi transport
        jobProcess.out --> hca.bmiIn++;
        jobProcess.in <-- hca.bmiOut++;

        // Connect forwarding gates
        ethIn --> hca.ethIn++;
        ethOut <-- hca.ethOut++;

endmodule
