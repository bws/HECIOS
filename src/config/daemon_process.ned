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
// A UNIX Daemon process
//
module DaemonProcess

    gates:
        in: in, fromOS;
        out: out, toOS;

    submodules:
        serverJobManager: JobManager;
            display: "p=120,80;i=block/classifier,light grey";

		requestScheduler: RequestScheduler;
			display: "p=60,180;i=block/queue,light grey";
			
        pfsServer: FSServer;
            display: "p=60,280;i=block/process,light grey";
          
        trove: StorageLayer;
            display: "p=260,80;i=block/buffer2,light sea green";
 
    connections nocheck:
        // Messages inbound to the job manager
        in --> serverJobManager.netIn;
        requestScheduler.requestOut --> serverJobManager.pfsIn;
        trove.out --> serverJobManager.storageIn;

        // Messages outbound from the job manager
        serverJobManager.pfsOut --> requestScheduler.requestIn;
        serverJobManager.storageOut --> trove.in;
        serverJobManager.netOut --> out;

		// Request scheduler to pfs server connections
		requestScheduler.serverOut --> pfsServer.in;
		pfsServer.out --> requestScheduler.serverIn;
		
        // Storage layer/OS connections
        trove.toOS --> toOS;
        trove.fromOS <-- fromOS;
endmodule

//
// Local variables:
//  indent-tabs-mode: nil
//  c-indent-level: 4
//  c-basic-offset: 4
// End:
//
// vim: ts=4 sts=4 sw=4 expandtab foldmethod=marker
//
