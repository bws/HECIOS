
module StorageLayer

    parameters:
        ioLibraryType : string,
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

        ioLibrary: ioLibraryType like IOLibrary;
            display: "p=80,80;i=block/layer,white";

        systemCall: systemCallInterfaceType like SystemCallInterface;
            display: "p=80,180;i=block/layer,green";

        fileSystem: fileSystemType like FileSystem;
            display: "p=80,280;i=block/wheelbarrow,yellow";

        blockTrans: blockTranslatorType like BlockTranslator;
            display: "p=80,380;i=block/classifier2,orange";

        bufferCache: bufferCacheType like BufferCache;
            display: "p=80,480;i=block/buffer,red";

        diskScheduler: diskSchedulerType like DiskScheduler;
            display: "p=80,580;i=block/control,purple";

        //            accessManager: accessManagerType like AccessManager;
        //    display: "p=80,680;i=block/cogwheel,blue";

    connections:

    in --> ioLibrary.in;
    out <-- ioLibrary.out;

    ioLibrary.request --> systemCall.in;
    ioLibrary.response <-- systemCall.out;

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
//  c-indent-level: 4
//  c-basic-offset: 4
// End:
//
// vim: ts=4 sts=4 sw=4 expandtab foldmethod=marker
//
