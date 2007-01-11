import
    "StandardHost",
    "Router",
    "FlatNetworkConfigurator";

channel fastlink
    datarate 8000000; // bits/sec
    delay 0.1ms;
endchannel

channel slowlink
    datarate 800000; // bits/sec
    delay 100ms;
endchannel

module file_sender_receiver
    submodules:
        configurator: FlatNetworkConfigurator;
            parameters:
                moduleTypes = "Router StandardHost",
                nonIPModuleTypes = "",
                networkAddress = "145.236.0.0",
                netmask = "255.255.0.0";
            display: "p=56,64;i=block/cogwheel";
        sender: StandardHost;
            display: "p=42,144;i=device/laptop";
        router: Router;
            display: "p=147,143;i=abstract/router";
        receiver: StandardHost;
            display: "p=252,144;i=device/server";
    connections nocheck:
        sender.out++ --> fastlink --> router.in++;
        sender.in++ <-- fastlink <-- router.out++;

        router.out++ --> slowlink --> receiver.in++;
        router.in++ <-- slowlink <-- receiver.out++;
endmodule

network file_sender_receiver_net : file_sender_receiver
endnetwork
