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
