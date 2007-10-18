//
// This file is part of Hecios
//
// Copyright (C) 2007 Brad Settlemyer
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
//

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
        job: JobProcess;
            display: "p=100,180;i=block/process,blue";

        // Note:  Statically, we are only able to construct TCPApps of a
        // single type, however the underlying TCPApp's will be
        // changed dynamically to be of the types:
        // 0.  BMITcpClient
        // 1.  MPITcpClient
        // 2.  MPITcpServer
        hca: EnhancedHost;
        parameters:
            numTcpApps = 3,
            tcpAppType = "BMITcpEndpoint";
        display: "p=100,80;i=block/rxtx,medium purple";

    connections:

        // Connect process to network transport
        for i=0..2 do
            job.out++ --> hca.bmiIn++;
            job.in++ <-- hca.bmiOut++;
        endfor

        // Connect forwarding gates
        ethIn --> hca.ethIn++;
        ethOut <-- hca.ethOut++;

endmodule

//
// Local variables:
//  c-indent-level: 4
//  c-basic-offset: 4
// End:
//
// vim: ts=4 sts=4 sw=4 expandtab foldmethod=marker
//
