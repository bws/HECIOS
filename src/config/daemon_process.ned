
import
    "bmi_tcp_server",
    "fs_server";

//
// A UNIX Daemon process
//
module DaemonProcess

    gates:
        in: in, fromDisk;
        out: out, toDisk;

    submodules:
        server: FSServer;
            display: "p=80,80;i=block/process,light grey";
          
        storage: StorageLayer;
            display: "p=80,180;i=block/buffer2,light sea green";
 
    connections nocheck:
        server.netOut --> out;
        server.netIn <-- in; 

        server.storageOut --> storage.in;
        server.storageIn <-- storage.out;

        storage.toDisk --> toDisk;
        storage.fromDisk <-- fromDisk;
endmodule

//
// Local variables:
//  c-indent-level: 4
//  c-basic-offset: 4
// End:
//
// vim: ts=4 sts=4 sw=4 expandtab foldmethod=marker
//
