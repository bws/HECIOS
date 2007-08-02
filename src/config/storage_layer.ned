
import
    "io_library",
    "system_call_interface",
    "file_system",
    "block_cache",
    "block_translator",
    "disk_scheduler",
    "access_manager",
    "hard_disk";

module StorageLayer

    parameters:
        ioLibraryType : string,
        systemCallInterfaceType : string,
        fileSystemType : string,
        blockCacheType : string,
        blockTranslatorType : string,
        diskSchedulerType : string,
        accessManagerType : string,
        physicalDiskType : string;

    gates:
        in: in, fromDisk;
        out: out, toDisk;

    submodules:

        ioLibrary: ioLibraryType like AIOLibrary;
            display: "p=80,80;i=block/layer,white";

        systemCall: systemCallInterfaceType like ASystemCallInterface;
            display: "p=80,180;i=block/layer,green";

        fileSystem: fileSystemType like AFileSystem;
            display: "p=80,280;i=block/wheelbarrow,forest green";

        blockCache: blockCacheType like ABlockCache;
            display: "p=80,380;i=block/buffer,yellow";

        blockTrans: blockTranslatorType like ABlockTranslator;
            display: "p=80,480;i=block/classifier2,orange";

        diskScheduler: diskSchedulerType like ADiskScheduler;
            display: "p=80,580;i=block/control,orange red";

        accessManager: accessManagerType like AAccessManager;
            display: "p=80,680;i=block/cogwheel,red";

        //hardDisk: physicalDiskType like BasicModelDisk;
        //    display: "p=80,780;i=block/buffer2,purple";

    connections:

    in --> ioLibrary.in;
    out <-- ioLibrary.out;

    ioLibrary.request --> systemCall.in;
    ioLibrary.response <-- systemCall.out;

    systemCall.request --> fileSystem.in;
    systemCall.response <-- fileSystem.out;

    fileSystem.request --> blockCache.in;
    fileSystem.response <-- blockCache.out;

    blockCache.request --> blockTrans.in;
    blockTrans.out --> diskScheduler.in;
    blockCache.response <-- diskScheduler.out;
    
    accessManager.get_next_request --> diskScheduler.get_next_request;
    diskScheduler.next_request --> accessManager.next_request;
    diskScheduler.response <-- accessManager.out;

    //accessManager.get_block --> hardDisk.in;
    //accessManager.block <-- hardDisk.out;

    accessManager.get_block --> toDisk;
    accessManager.block <-- fromDisk;

endmodule

//
// Local variables:
//  c-indent-level: 4
//  c-basic-offset: 4
// End:
//
// vim: ts=4 sts=4 sw=4 expandtab foldmethod=marker
//
