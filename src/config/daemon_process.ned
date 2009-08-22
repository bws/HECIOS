//
// This file is part of Hecios
//
// Copyright (C) 2007,2008,2009 Brad Settlemyer
//
// This file is distributed WITHOUT ANY WARRANTY. See the file 'License.txt'
// for details on this and other legal matters.
//

//
// A UNIX Daemon process
//
module DaemonProcess

{
    gates:
        input in;
        input fromOS;
        output out;
        output toOS;

    submodules:
        serverJobManager: JobManager {
            parameters:
                @display("p=120,80;i=block/classifier,light grey");

        }
        requestScheduler: RequestScheduler {
            parameters:
                @display("p=60,180;i=block/queue,light grey");

        }
        pfsServer: FSServer {
            parameters:
                @display("p=60,280;i=block/process,light grey");

        }
        trove: StorageLayer {
            parameters:
                @display("p=260,80;i=block/buffer2,light sea green");

        }
    connections allowunconnected:
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
}

