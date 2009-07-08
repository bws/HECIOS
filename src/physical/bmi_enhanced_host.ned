//
// This file is part of Hecios
//
// Copyright (C) 2006 Andras Vargas
// Copyright (C) 2007 Brad Settlemyer
//
// This file is distributed WITHOUT ANY WARRANTY. See the file 'License.txt'
// for details on this and other legal matters.
//

//
// A replacement for INET's standard host that includes additional
// gates for connecting to the tcpApp's messaging gates in the case
// of a BMI application
//

import
    "NotificationBoard",
    "NAMTraceWriter",
    "InterfaceTable",
    "RoutingTable",
    "TCPApp.ned",
    "TCP.ned",
    "UDP",
    "UDPApp",
    "NetworkLayer",
    "PingApp",
    "PPPInterface",
    "EthernetInterface";



//
// An embellishment of INET's StandardHost that also provides BMI support
// in the TCP Application.
//
module BMIEnhancedHost
    parameters:
        numTcpApps: numeric const,
        numUdpApps: numeric const,
        tcpAppType: string,
        udpAppType: string,
        IPForward: bool,
        namid: numeric const,
        routingFile: string;
    gates:
        in: in[];
        out: out[];
        in: ethIn[];
        out: ethOut[];

        // Additional gates for bmi applications
        in: bmiIn[];
        out: bmiOut[];

    submodules:
        namTrace: NAMTraceWriter;
            parameters:
                namid = namid;
            display: "p=60,310;i=block/sink";
        notificationBoard: NotificationBoard;
            display: "p=60,70;i=block/control";
        interfaceTable: InterfaceTable;
            display: "p=60,150;i=block/table";
        routingTable: RoutingTable;
            parameters:
                IPForward = IPForward,
                routerId = "",
                routingFile = routingFile;
            display: "p=60,230;i=block/table";
        bmiTcpApp: tcpAppType[numTcpApps] like TCPApp;
            display: "p=163,67;i=block/app";
        tcp: TCP;
            display: "p=163,154;i=block/wheelbarrow";
        udpApp: udpAppType[numUdpApps] like UDPApp;
            display: "i=block/app;p=272,67";
        udp: UDP;
            display: "p=272,154;i=block/transport";
        pingApp: PingApp;
            display: "i=block/app;p=343,200";
        networkLayer: NetworkLayer;
            parameters:
                proxyARP = false;
            gatesizes:
                ifIn[sizeof(out)+sizeof(ethOut)],
                ifOut[sizeof(out)+sizeof(ethOut)];
            display: "p=248,247;i=block/fork;q=queue";
        ppp: PPPInterface[sizeof(out)];
            display: "p=205,350,row,90;q=txQueue;i=block/ifcard";
        eth: EnhancedEthernetInterface2[sizeof(ethOut)];
            display: "p=240,350,row,90;q=txQueue;i=block/ifcard";
    connections nocheck:
        for i=0..numTcpApps-1 do
            bmiTcpApp[i].tcpOut --> tcp.from_appl++;
            bmiTcpApp[i].tcpIn <-- tcp.to_appl++;
        endfor;

        tcp.to_ip --> networkLayer.TCPIn;
        tcp.from_ip <-- networkLayer.TCPOut;

        for i=0..numUdpApps-1 do
            udpApp[i].to_udp --> udp.from_app++;
            udpApp[i].from_udp <-- udp.to_app++;
        endfor;

        udp.to_ip --> networkLayer.UDPIn;
        udp.from_ip <-- networkLayer.UDPOut;

        networkLayer.pingOut --> pingApp.pingIn;
        networkLayer.pingIn <-- pingApp.pingOut;

        // connections to network outside
        for i=0..sizeof(out)-1 do
            in[i] --> ppp[i].physIn;
            out[i] <-- ppp[i].physOut;
            ppp[i].netwOut --> networkLayer.ifIn[i];
            ppp[i].netwIn <-- networkLayer.ifOut[i];
        endfor;

        for i=0..sizeof(ethOut)-1 do
            ethIn[i] --> eth[i].physIn;
            ethOut[i] <-- eth[i].physOut;
            eth[i].netwOut --> networkLayer.ifIn[sizeof(out)+i];
            eth[i].netwIn <-- networkLayer.ifOut[sizeof(out)+i];
        endfor;

        // Connections for BMI messaging applications
        for i=0..numTcpApps-1 do
            bmiIn[i] --> bmiTcpApp[i].appIn;
            bmiOut[i] <-- bmiTcpApp[i].appOut;
        endfor;

endmodule
