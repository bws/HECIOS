//
// This file is part of Hecios
//
// Copyright (C) 2007,2008,2009 Brad Settlemyer
//
// This file is distributed WITHOUT ANY WARRANTY. See the file 'License.txt'
// for details on this and other legal matters.
//

module OperatingSystem

    parameters:
        systemCallInterfaceType : string,
        fileSystemType : string,
        blockTranslatorType : string,
        bufferCacheType : string,
        diskSchedulerType : string,
        accessManagerType : string;

    gates:
        in: in, fromDisk;
        out: out, toDisk;

    submodules:

        systemCall: systemCallInterfaceType like SystemCallInterface;
            display: "p=80,80;i=block/layer,green";

        fileSystem: fileSystemType like FileSystem;
            display: "p=80,180;i=block/wheelbarrow,yellow";

        blockTrans: blockTranslatorType like BlockTranslator;
            display: "p=80,280;i=block/classifier2,orange";

        bufferCache: bufferCacheType like BufferCache;
            display: "p=80,380;i=block/buffer,red";

        diskScheduler: diskSchedulerType like DiskScheduler;
            display: "p=80,480;i=block/control,purple";

        //            accessManager: accessManagerType like AccessManager;
        //    display: "p=80,680;i=block/cogwheel,blue";

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
