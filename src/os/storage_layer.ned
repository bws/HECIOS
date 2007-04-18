
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
        in: in;
        out: out;

    submodules:

        ioLibrary: ioLibraryType like AIOLibrary;

        systemCall: systemCallInterfaceType like ASystemCallInterface;

        fileSystem: fileSystemType like AFileSystem;

        blockCache: blockCacheType like ABlockCache;

        blockTrans: blockTranslatorType like ABlockTranslator;

        diskScheduler: diskSchedulerType like ADiskScheduler;

        accessManager: accessManagerType like AAccessManager;

        hardDisk: physicalDiskType like BasicModelDisk;

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

    accessManager.get_block --> hardDisk.in;
    accessManager.block <-- hardDisk.out;

endmodule