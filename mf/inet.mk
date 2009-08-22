#
# This file is part of Hecios
#
# Copyright (C) 2007,2008,2009 Brad Settlemyer
#
# This file is distributed WITHOUT ANY WARRANTY. See the file 'License.txt'
# for details on this and other legal matters.
#

#
# Makefile to build the INET framework and assemble it for use
# within the Hecios simulator
#

#
# INET locations
#
INET_OBJ_DIR = $(INET_DIR)/out/gcc-release/src

#
# INET framework object files for use in Hecios
#
INET_OBJS = $(INET_OBJ_DIR)/applications/ethernet/EtherApp_m.o \
	$(INET_OBJ_DIR)/applications/ethernet/EtherAppCli.o \
	$(INET_OBJ_DIR)/applications/ethernet/EtherAppSrv.o \
	$(INET_OBJ_DIR)/applications/generic/IPTrafGen.o \
	$(INET_OBJ_DIR)/applications/pingapp/PingPayload_m.o \
	$(INET_OBJ_DIR)/applications/pingapp/PingApp.o \
	$(INET_OBJ_DIR)/applications/tcpapp/GenericAppMsg_m.o \
	$(INET_OBJ_DIR)/applications/tcpapp/TCPBasicClientApp.o \
	$(INET_OBJ_DIR)/applications/tcpapp/TCPEchoApp.o \
	$(INET_OBJ_DIR)/applications/tcpapp/TCPGenericCliAppBase.o \
	$(INET_OBJ_DIR)/applications/tcpapp/TCPGenericSrvApp.o \
	$(INET_OBJ_DIR)/applications/tcpapp/TCPGenericSrvThread.o \
	$(INET_OBJ_DIR)/applications/tcpapp/TCPSessionApp.o \
	$(INET_OBJ_DIR)/applications/tcpapp/TCPSinkApp.o \
	$(INET_OBJ_DIR)/applications/tcpapp/TCPSpoof.o \
	$(INET_OBJ_DIR)/applications/tcpapp/TCPSrvHostApp.o \
	$(INET_OBJ_DIR)/applications/tcpapp/TelnetApp.o \
	$(INET_OBJ_DIR)/applications/udpapp/UDPEchoAppMsg_m.o \
	$(INET_OBJ_DIR)/applications/udpapp/UDPAppBase.o \
	$(INET_OBJ_DIR)/applications/udpapp/UDPBasicApp.o \
	$(INET_OBJ_DIR)/applications/udpapp/UDPEchoApp.o \
	$(INET_OBJ_DIR)/applications/udpapp/UDPSink.o \
	$(INET_OBJ_DIR)/applications/udpapp/UDPVideoStreamCli.o \
	$(INET_OBJ_DIR)/applications/udpapp/UDPVideoStreamSvr.o \
	$(INET_OBJ_DIR)/base/ByteArrayMessage_m.o \
	$(INET_OBJ_DIR)/base/AbstractQueue.o \
	$(INET_OBJ_DIR)/base/BasicModule.o \
	$(INET_OBJ_DIR)/base/Blackboard.o \
	$(INET_OBJ_DIR)/base/ByteArrayMessage.o \
	$(INET_OBJ_DIR)/base/Join.o \
	$(INET_OBJ_DIR)/base/ModuleAccess.o \
	$(INET_OBJ_DIR)/base/NotificationBoard.o \
	$(INET_OBJ_DIR)/base/NotifierConsts.o \
	$(INET_OBJ_DIR)/base/PassiveQueueBase.o \
	$(INET_OBJ_DIR)/base/ProtocolMap.o \
	$(INET_OBJ_DIR)/base/QueueBase.o \
	$(INET_OBJ_DIR)/base/QueueWithQoS.o \
	$(INET_OBJ_DIR)/base/ReassemblyBuffer.o \
	$(INET_OBJ_DIR)/base/Sink.o \
	$(INET_OBJ_DIR)/mobility/ANSimMobility.o \
	$(INET_OBJ_DIR)/mobility/BasicMobility.o \
	$(INET_OBJ_DIR)/mobility/BonnMotionFileCache.o \
	$(INET_OBJ_DIR)/mobility/BonnMotionMobility.o \
	$(INET_OBJ_DIR)/mobility/CircleMobility.o \
	$(INET_OBJ_DIR)/mobility/ConstSpeedMobility.o \
	$(INET_OBJ_DIR)/mobility/LinearMobility.o \
	$(INET_OBJ_DIR)/mobility/LineSegmentsMobilityBase.o \
	$(INET_OBJ_DIR)/mobility/MassMobility.o \
	$(INET_OBJ_DIR)/mobility/NullMobility.o \
	$(INET_OBJ_DIR)/mobility/RandomWPMobility.o \
	$(INET_OBJ_DIR)/mobility/RectangleMobility.o \
	$(INET_OBJ_DIR)/mobility/TurtleMobility.o \
	$(INET_OBJ_DIR)/networklayer/arp/ARPPacket_m.o \
	$(INET_OBJ_DIR)/networklayer/arp/ARP.o \
	$(INET_OBJ_DIR)/networklayer/autorouting/FlatNetworkConfigurator6.o \
	$(INET_OBJ_DIR)/networklayer/autorouting/FlatNetworkConfigurator.o \
	$(INET_OBJ_DIR)/networklayer/autorouting/NetworkConfigurator.o \
	$(INET_OBJ_DIR)/networklayer/common/InterfaceEntry.o \
	$(INET_OBJ_DIR)/networklayer/common/InterfaceTable.o \
	$(INET_OBJ_DIR)/networklayer/contract/IPControlInfo_m.o \
	$(INET_OBJ_DIR)/networklayer/contract/IPProtocolId_m.o \
	$(INET_OBJ_DIR)/networklayer/contract/IPv6ControlInfo_m.o \
	$(INET_OBJ_DIR)/networklayer/contract/IPAddress.o \
	$(INET_OBJ_DIR)/networklayer/contract/IPAddressResolver.o \
	$(INET_OBJ_DIR)/networklayer/contract/IPControlInfo.o \
	$(INET_OBJ_DIR)/networklayer/contract/IPv6Address.o \
	$(INET_OBJ_DIR)/networklayer/contract/IPv6ControlInfo.o \
	$(INET_OBJ_DIR)/networklayer/contract/IPvXAddress.o \
	$(INET_OBJ_DIR)/networklayer/extras/Dummy.o \
	$(INET_OBJ_DIR)/networklayer/extras/FailureManager.o \
	$(INET_OBJ_DIR)/networklayer/icmpv6/ICMPv6Message_m.o \
	$(INET_OBJ_DIR)/networklayer/icmpv6/IPv6NDMessage_m.o \
	$(INET_OBJ_DIR)/networklayer/icmpv6/ICMPv6.o \
	$(INET_OBJ_DIR)/networklayer/icmpv6/IPv6NeighbourCache.o \
	$(INET_OBJ_DIR)/networklayer/icmpv6/IPv6NeighbourDiscovery.o \
	$(INET_OBJ_DIR)/networklayer/ipv4/ControlManetRouting_m.o \
	$(INET_OBJ_DIR)/networklayer/ipv4/ICMPMessage_m.o \
	$(INET_OBJ_DIR)/networklayer/ipv4/IPDatagram_m.o \
	$(INET_OBJ_DIR)/networklayer/ipv4/ErrorHandling.o \
	$(INET_OBJ_DIR)/networklayer/ipv4/ICMP.o \
	$(INET_OBJ_DIR)/networklayer/ipv4/IP.o \
	$(INET_OBJ_DIR)/networklayer/ipv4/IPFragBuf.o \
	$(INET_OBJ_DIR)/networklayer/ipv4/IPv4InterfaceData.o \
	$(INET_OBJ_DIR)/networklayer/ipv4/RoutingTable.o \
	$(INET_OBJ_DIR)/networklayer/ipv4/RoutingTableParser.o \
	$(INET_OBJ_DIR)/networklayer/ipv6/IPv6Datagram_m.o \
	$(INET_OBJ_DIR)/networklayer/ipv6/IPv6ExtensionHeaders_m.o \
	$(INET_OBJ_DIR)/networklayer/ipv6/IPv6.o \
	$(INET_OBJ_DIR)/networklayer/ipv6/IPv6Datagram.o \
	$(INET_OBJ_DIR)/networklayer/ipv6/IPv6ErrorHandling.o \
	$(INET_OBJ_DIR)/networklayer/ipv6/IPv6FragBuf.o \
	$(INET_OBJ_DIR)/networklayer/ipv6/IPv6InterfaceData.o \
	$(INET_OBJ_DIR)/networklayer/ipv6/RoutingTable6.o \
	$(INET_OBJ_DIR)/networklayer/ldp/LDPPacket_m.o \
	$(INET_OBJ_DIR)/networklayer/ldp/LDP.o \
	$(INET_OBJ_DIR)/networklayer/mpls/LIBTable.o \
	$(INET_OBJ_DIR)/networklayer/mpls/MPLS.o \
	$(INET_OBJ_DIR)/networklayer/mpls/MPLSPacket.o \
	$(INET_OBJ_DIR)/networklayer/ospfv2/interface/OSPFInterface.o \
	$(INET_OBJ_DIR)/networklayer/ospfv2/interface/OSPFInterfaceStateBackup.o \
	$(INET_OBJ_DIR)/networklayer/ospfv2/interface/OSPFInterfaceState.o \
	$(INET_OBJ_DIR)/networklayer/ospfv2/interface/OSPFInterfaceStateDesignatedRouter.o \
	$(INET_OBJ_DIR)/networklayer/ospfv2/interface/OSPFInterfaceStateDown.o \
	$(INET_OBJ_DIR)/networklayer/ospfv2/interface/OSPFInterfaceStateLoopback.o \
	$(INET_OBJ_DIR)/networklayer/ospfv2/interface/OSPFInterfaceStateNotDesignatedRouter.o \
	$(INET_OBJ_DIR)/networklayer/ospfv2/interface/OSPFInterfaceStatePointToPoint.o \
	$(INET_OBJ_DIR)/networklayer/ospfv2/interface/OSPFInterfaceStateWaiting.o \
	$(INET_OBJ_DIR)/networklayer/ospfv2/messagehandler/DatabaseDescriptionHandler.o \
	$(INET_OBJ_DIR)/networklayer/ospfv2/messagehandler/HelloHandler.o \
	$(INET_OBJ_DIR)/networklayer/ospfv2/messagehandler/LinkStateAcknowledgementHandler.o \
	$(INET_OBJ_DIR)/networklayer/ospfv2/messagehandler/LinkStateRequestHandler.o \
	$(INET_OBJ_DIR)/networklayer/ospfv2/messagehandler/LinkStateUpdateHandler.o \
	$(INET_OBJ_DIR)/networklayer/ospfv2/messagehandler/MessageHandler.o \
	$(INET_OBJ_DIR)/networklayer/ospfv2/neighbor/OSPFNeighbor.o \
	$(INET_OBJ_DIR)/networklayer/ospfv2/neighbor/OSPFNeighborStateAttempt.o \
	$(INET_OBJ_DIR)/networklayer/ospfv2/neighbor/OSPFNeighborState.o \
	$(INET_OBJ_DIR)/networklayer/ospfv2/neighbor/OSPFNeighborStateDown.o \
	$(INET_OBJ_DIR)/networklayer/ospfv2/neighbor/OSPFNeighborStateExchange.o \
	$(INET_OBJ_DIR)/networklayer/ospfv2/neighbor/OSPFNeighborStateExchangeStart.o \
	$(INET_OBJ_DIR)/networklayer/ospfv2/neighbor/OSPFNeighborStateFull.o \
	$(INET_OBJ_DIR)/networklayer/ospfv2/neighbor/OSPFNeighborStateInit.o \
	$(INET_OBJ_DIR)/networklayer/ospfv2/neighbor/OSPFNeighborStateLoading.o \
	$(INET_OBJ_DIR)/networklayer/ospfv2/neighbor/OSPFNeighborStateTwoWay.o \
	$(INET_OBJ_DIR)/networklayer/ospfv2/router/ASExternalLSA.o \
	$(INET_OBJ_DIR)/networklayer/ospfv2/router/NetworkLSA.o \
	$(INET_OBJ_DIR)/networklayer/ospfv2/router/OSPFArea.o \
	$(INET_OBJ_DIR)/networklayer/ospfv2/router/OSPFRouter.o \
	$(INET_OBJ_DIR)/networklayer/ospfv2/router/RouterLSA.o \
	$(INET_OBJ_DIR)/networklayer/ospfv2/router/SummaryLSA.o \
	$(INET_OBJ_DIR)/networklayer/ospfv2/OSPFPacket_m.o \
	$(INET_OBJ_DIR)/networklayer/ospfv2/OSPFTimer_m.o \
	$(INET_OBJ_DIR)/networklayer/ospfv2/OSPFRouting.o \
	$(INET_OBJ_DIR)/networklayer/queue/BasicDSCPClassifier.o \
	$(INET_OBJ_DIR)/networklayer/queue/DropTailQoSQueue.o \
	$(INET_OBJ_DIR)/networklayer/queue/DropTailQueue.o \
	$(INET_OBJ_DIR)/networklayer/queue/REDQueue.o \
	$(INET_OBJ_DIR)/networklayer/rsvp_te/IntServ_m.o \
	$(INET_OBJ_DIR)/networklayer/rsvp_te/RSVPHello_m.o \
	$(INET_OBJ_DIR)/networklayer/rsvp_te/RSVPPacket_m.o \
	$(INET_OBJ_DIR)/networklayer/rsvp_te/RSVPPathMsg_m.o \
	$(INET_OBJ_DIR)/networklayer/rsvp_te/RSVPResvMsg_m.o \
	$(INET_OBJ_DIR)/networklayer/rsvp_te/SignallingMsg_m.o \
	$(INET_OBJ_DIR)/networklayer/rsvp_te/RSVP.o \
	$(INET_OBJ_DIR)/networklayer/rsvp_te/SimpleClassifier.o \
	$(INET_OBJ_DIR)/networklayer/rsvp_te/Utils.o \
	$(INET_OBJ_DIR)/networklayer/ted/LinkStatePacket_m.o \
	$(INET_OBJ_DIR)/networklayer/ted/TED_m.o \
	$(INET_OBJ_DIR)/networklayer/ted/LinkStateRouting.o \
	$(INET_OBJ_DIR)/networklayer/ted/TED.o \
	$(INET_OBJ_DIR)/linklayer/contract/Ieee802Ctrl_m.o \
	$(INET_OBJ_DIR)/linklayer/contract/PhyControlInfo_m.o \
	$(INET_OBJ_DIR)/linklayer/contract/MACAddress.o \
	$(INET_OBJ_DIR)/linklayer/etherswitch/MACRelayUnitBase.o \
	$(INET_OBJ_DIR)/linklayer/etherswitch/MACRelayUnitNP.o \
	$(INET_OBJ_DIR)/linklayer/etherswitch/MACRelayUnitPP.o \
	$(INET_OBJ_DIR)/linklayer/ethernet/EtherFrame_m.o \
	$(INET_OBJ_DIR)/linklayer/ethernet/EtherBus.o \
	$(INET_OBJ_DIR)/linklayer/ethernet/EtherEncap.o \
	$(INET_OBJ_DIR)/linklayer/ethernet/EtherHub.o \
	$(INET_OBJ_DIR)/linklayer/ethernet/EtherLLC.o \
	$(INET_OBJ_DIR)/linklayer/ethernet/EtherMAC2.o \
	$(INET_OBJ_DIR)/linklayer/ethernet/EtherMACBase.o \
	$(INET_OBJ_DIR)/linklayer/ethernet/EtherMAC.o \
	$(INET_OBJ_DIR)/linklayer/ieee80211/mac/Ieee80211Frame_m.o \
	$(INET_OBJ_DIR)/linklayer/ieee80211/mac/Ieee80211Mac.o \
	$(INET_OBJ_DIR)/linklayer/ieee80211/mgmt/Ieee80211MgmtFrames_m.o \
	$(INET_OBJ_DIR)/linklayer/ieee80211/mgmt/Ieee80211Primitives_m.o \
	$(INET_OBJ_DIR)/linklayer/ieee80211/mgmt/Ieee80211AgentSTA.o \
	$(INET_OBJ_DIR)/linklayer/ieee80211/mgmt/Ieee80211MgmtAdhoc.o \
	$(INET_OBJ_DIR)/linklayer/ieee80211/mgmt/Ieee80211MgmtAPBase.o \
	$(INET_OBJ_DIR)/linklayer/ieee80211/mgmt/Ieee80211MgmtAP.o \
	$(INET_OBJ_DIR)/linklayer/ieee80211/mgmt/Ieee80211MgmtAPSimplified.o \
	$(INET_OBJ_DIR)/linklayer/ieee80211/mgmt/Ieee80211MgmtBase.o \
	$(INET_OBJ_DIR)/linklayer/ieee80211/mgmt/Ieee80211MgmtSTA.o \
	$(INET_OBJ_DIR)/linklayer/ieee80211/mgmt/Ieee80211MgmtSTASimplified.o \
	$(INET_OBJ_DIR)/linklayer/mf80211/macLayer/Mac80211Pkt_m.o \
	$(INET_OBJ_DIR)/linklayer/mf80211/macLayer/MacPkt_m.o \
	$(INET_OBJ_DIR)/linklayer/mf80211/macLayer/CSMAMacLayer.o \
	$(INET_OBJ_DIR)/linklayer/mf80211/macLayer/Mac80211.o \
	$(INET_OBJ_DIR)/linklayer/mf80211/phyLayer/decider/Decider80211.o \
	$(INET_OBJ_DIR)/linklayer/mf80211/phyLayer/decider/ErrAndCollDecider.o \
	$(INET_OBJ_DIR)/linklayer/mf80211/phyLayer/decider/SnrDecider.o \
	$(INET_OBJ_DIR)/linklayer/mf80211/phyLayer/snrEval/GilbertElliotSnr.o \
	$(INET_OBJ_DIR)/linklayer/mf80211/phyLayer/snrEval/SnrEval80211.o \
	$(INET_OBJ_DIR)/linklayer/mf80211/phyLayer/snrEval/SnrEval.o \
	$(INET_OBJ_DIR)/linklayer/mfcore/AirFrame_m.o \
	$(INET_OBJ_DIR)/linklayer/mfcore/SnrControlInfo_m.o \
	$(INET_OBJ_DIR)/linklayer/mfcore/TransmComplete_m.o \
	$(INET_OBJ_DIR)/linklayer/mfcore/BasicDecider.o \
	$(INET_OBJ_DIR)/linklayer/mfcore/BasicSnrEval.o \
	$(INET_OBJ_DIR)/linklayer/mfcore/WirelessMacBase.o \
	$(INET_OBJ_DIR)/linklayer/ppp/PPPFrame_m.o \
	$(INET_OBJ_DIR)/linklayer/ppp/PPP.o \
	$(INET_OBJ_DIR)/linklayer/ppp/ThruputMeter.o \
	$(INET_OBJ_DIR)/linklayer/radio/AbstractRadio.o \
	$(INET_OBJ_DIR)/linklayer/radio/GenericRadio.o \
	$(INET_OBJ_DIR)/linklayer/radio/GenericRadioModel.o \
	$(INET_OBJ_DIR)/linklayer/radio/Ieee80211Radio.o \
	$(INET_OBJ_DIR)/linklayer/radio/Ieee80211RadioModel.o \
	$(INET_OBJ_DIR)/linklayer/radio/Modulation.o \
	$(INET_OBJ_DIR)/linklayer/radio/PathLossReceptionModel.o \
	$(INET_OBJ_DIR)/transport/contract/TCPCommand_m.o \
	$(INET_OBJ_DIR)/transport/contract/UDPControlInfo_m.o \
	$(INET_OBJ_DIR)/transport/contract/TCPSocket.o \
	$(INET_OBJ_DIR)/transport/contract/TCPSocketMap.o \
	$(INET_OBJ_DIR)/transport/contract/UDPSocket.o \
	$(INET_OBJ_DIR)/transport/tcp/flavours/DumbTCP.o \
	$(INET_OBJ_DIR)/transport/tcp/flavours/TCPBaseAlg.o \
	$(INET_OBJ_DIR)/transport/tcp/flavours/TCPNoCongestionControl.o \
	$(INET_OBJ_DIR)/transport/tcp/flavours/TCPReno.o \
	$(INET_OBJ_DIR)/transport/tcp/flavours/TCPTahoe.o \
	$(INET_OBJ_DIR)/transport/tcp/flavours/TCPTahoeRenoFamily.o \
	$(INET_OBJ_DIR)/transport/tcp/queues/TCPMsgBasedRcvQueue.o \
	$(INET_OBJ_DIR)/transport/tcp/queues/TCPMsgBasedSendQueue.o \
	$(INET_OBJ_DIR)/transport/tcp/queues/TCPVirtualDataRcvQueue.o \
	$(INET_OBJ_DIR)/transport/tcp/queues/TCPVirtualDataSendQueue.o \
	$(INET_OBJ_DIR)/transport/tcp/TCPSegment_m.o \
	$(INET_OBJ_DIR)/transport/tcp/TCP.o \
	$(INET_OBJ_DIR)/transport/tcp/TCPConnectionBase.o \
	$(INET_OBJ_DIR)/transport/tcp/TCPConnectionEventProc.o \
	$(INET_OBJ_DIR)/transport/tcp/TCPConnectionRcvSegment.o \
	$(INET_OBJ_DIR)/transport/tcp/TCPConnectionUtil.o \
	$(INET_OBJ_DIR)/transport/tcp/TCPSegment.o \
	$(INET_OBJ_DIR)/transport/udp/UDPPacket_m.o \
	$(INET_OBJ_DIR)/transport/udp/UDP.o \
	$(INET_OBJ_DIR)/util/headerserializers/ICMPSerializer.o \
	$(INET_OBJ_DIR)/util/headerserializers/IPSerializer.o \
	$(INET_OBJ_DIR)/util/headerserializers/UDPSerializer.o \
	$(INET_OBJ_DIR)/util/common.o \
	$(INET_OBJ_DIR)/util/NAMTraceWriter.o \
	$(INET_OBJ_DIR)/util/opp_utils.o \
	$(INET_OBJ_DIR)/util/TCPDump.o \
	$(INET_OBJ_DIR)/util/ThruputMeteringChannel.o \
	$(INET_OBJ_DIR)/util/XMLUtils.o \
	$(INET_OBJ_DIR)/world/ChannelAccess.o \
	$(INET_OBJ_DIR)/world/ChannelControl.o \
	$(INET_OBJ_DIR)/world/ChannelInstaller.o \
	$(INET_OBJ_DIR)/world/NAMTrace.o \
	$(INET_OBJ_DIR)/world/ScenarioManager.o

INET_LIBS = 

#	$(LIB_DIR)/libospfd.a \
#	$(LIB_DIR)/libripd.a \
#	$(LIB_DIR)/libzebra.a \
#	$(LIB_DIR)/libzebra2.a

INET_CMD_LIBS = -lenvir -lcmdenv

INET_OUT_DIR = $(INET_DIR)/out/gcc-release/src

INET_TOPLEVEL_TARGET = $(INET_OUT_DIR)/libinet.so

$(INET_TOPLEVEL_TARGET):
	cd INET && PATH=$(OMNET_DIR)/bin:$$PATH && make makefiles
	cd INET && PATH=$(OMNET_DIR)/bin:$$PATH && make MODE=release

#
# A little hack to get the inet objects to build first
#
$(INET_OBJS): $(INET_TOPLEVEL_TARGET)

#
# Archives created by and for the INET framework package
#
lib/libospfd.a: $(INET_TOPLEVEL_TARGET)
	mkdir -p lib
	$(CP) $(INET_DIR)/Network/quagga/quaggasrc/quagga/ospfd/ospfd.a $@

lib/libripd.a: $(INET_TOPLEVEL_TARGET)
	mkdir -p lib
	$(CP) $(INET_DIR)/Network/quagga/quaggasrc/quagga/ripd/ripd.a $@

lib/libzebra2.a: $(INET_TOPLEVEL_TARGET)
	mkdir -p lib
	$(CP) $(INET_DIR)/Network/quagga/quaggasrc/quagga/zebra/zebra.a $@

lib/libzebra.a: $(INET_TOPLEVEL_TARGET)
	mkdir -p lib
	$(CP) $(INET_DIR)/Network/quagga/quaggasrc/quagga/lib/libzebra.a $@

#
# INET prelinked object file
#
lib/inet.o: $(INET_TOPLEVEL_TARGET) $(INET_LIBS) lib/libinet.so
	mkdir -p lib
	$(PRELINK) -L/lib -L/usr/lib -Llib $(INET_OBJS) -o $@

lib/libinet.so: $(INET_OUT_DIR)/libinet.so
	mkdir -p lib
	$(CP) $(INET_OUT_DIR)/libinet.so $@
	
#
# Clean out INET build
#
inet_clean:
	cd INET && make ROOT=$(shell pwd)/$(INET_DIR) clean
	$(RM) lib/inet.o $(INET_LIBS)

.PHONY: inet_clean
