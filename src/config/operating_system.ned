//
// This file is part of Hecios
//
// Copyright (C) 2007,2008,2009 Brad Settlemyer
//
// This file is distributed WITHOUT ANY WARRANTY. See the file 'License.txt'
// for details on this and other legal matters.
//

module OperatingSystem

{
    parameters:
        string systemCallInterfaceType;
        string fileSystemType;
        string blockTranslatorType;
        string bufferCacheType;
        string diskSchedulerType;
        string accessManagerType;

    gates:
        input in;
        input fromDisk;
        output out;
        output toDisk;

    submodules:

        systemCall: <systemCallInterfaceType> like SystemCallInterface {
            parameters:
                @display("p=80,80;i=block/layer,green");

        }
        fileSystem: <fileSystemType> like FileSystem {
            parameters:
                @display("p=80,180;i=block/wheelbarrow,yellow");

        }
        blockTrans: <blockTranslatorType> like BlockTranslator {
            parameters:
                @display("p=80,280;i=block/classifier2,orange");

        }
        bufferCache: <bufferCacheType> like BufferCache {
            parameters:
                @display("p=80,380;i=block/buffer,red");

        }
        diskScheduler: <diskSchedulerType> like DiskScheduler {
            parameters:
                @display("p=80,480;i=block/control,purple");

        //            accessManager: accessManagerType like AccessManager;
        //    display: "p=80,680;i=block/cogwheel,blue";

        }
    connections:

        in --> systemCall.in;
        out <-- systemCall.out;

        systemCall.request --> fileSystem.in;
        systemCall.response <-- fileSystem.out;

        fileSystem.request --> blockTrans.in;
        fileSystem.response <-- blockTrans.out;

        blockTrans.request --> bufferCache.in;
        blockTrans.response <-- bufferCache.out;

        bufferCache.request --> diskScheduler.in;
        bufferCache.response <-- diskScheduler.out;

        diskScheduler.request --> toDisk;
        diskScheduler.response <-- fromDisk;

}

