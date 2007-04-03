#
# Makefile to build the inter framework and assemble it for use
# within the Hecios simulator
#

#
# INET framework object files for use in Hecios
#
INET_OBJS = $(INET_DIR)/Applications/Ethernet/EtherApp_m.o \
	$(INET_DIR)/Applications/Ethernet/EtherAppCli.o \
	$(INET_DIR)/Applications/Ethernet/EtherAppSrv.o \
	$(INET_DIR)/Applications/Generic/IPTrafGen.o \
	$(INET_DIR)/Applications/PingApp/PingPayload_m.o \
	$(INET_DIR)/Applications/PingApp/PingApp.o \
	$(INET_DIR)/Applications/TCPApp/GenericAppMsg_m.o \
	$(INET_DIR)/Applications/TCPApp/TCPBasicClientApp.o \
	$(INET_DIR)/Applications/TCPApp/TCPEchoApp.o \
	$(INET_DIR)/Applications/TCPApp/TCPGenericCliAppBase.o \
	$(INET_DIR)/Applications/TCPApp/TCPGenericSrvApp.o \
	$(INET_DIR)/Applications/TCPApp/TCPGenericSrvThread.o \
	$(INET_DIR)/Applications/TCPApp/TCPSessionApp.o \
	$(INET_DIR)/Applications/TCPApp/TCPSinkApp.o \
	$(INET_DIR)/Applications/TCPApp/TCPSpoof.o \
	$(INET_DIR)/Applications/TCPApp/TCPSrvHostApp.o \
	$(INET_DIR)/Applications/TCPApp/TelnetApp.o \
	$(INET_DIR)/Applications/UDPApp/UDPEchoAppMsg_m.o \
	$(INET_DIR)/Applications/UDPApp/UDPAppBase.o \
	$(INET_DIR)/Applications/UDPApp/UDPBasicApp.o \
	$(INET_DIR)/Applications/UDPApp/UDPEchoApp.o \
	$(INET_DIR)/Applications/UDPApp/UDPSink.o \
	$(INET_DIR)/Applications/UDPApp/UDPVideoStreamCli.o \
	$(INET_DIR)/Applications/UDPApp/UDPVideoStreamSvr.o \
	$(INET_DIR)/Base/ByteArrayMessage_m.o \
	$(INET_DIR)/Base/AbstractQueue.o \
	$(INET_DIR)/Base/BasicModule.o \
	$(INET_DIR)/Base/Blackboard.o \
	$(INET_DIR)/Base/ByteArrayMessage.o \
	$(INET_DIR)/Base/Join.o \
	$(INET_DIR)/Base/ModuleAccess.o \
	$(INET_DIR)/Base/NotificationBoard.o \
	$(INET_DIR)/Base/NotifierConsts.o \
	$(INET_DIR)/Base/PassiveQueueBase.o \
	$(INET_DIR)/Base/ProtocolMap.o \
	$(INET_DIR)/Base/QueueBase.o \
	$(INET_DIR)/Base/QueueWithQoS.o \
	$(INET_DIR)/Base/ReassemblyBuffer.o \
	$(INET_DIR)/Base/Sink.o \
	$(INET_DIR)/Mobility/ANSimMobility.o \
	$(INET_DIR)/Mobility/BasicMobility.o \
	$(INET_DIR)/Mobility/BonnMotionFileCache.o \
	$(INET_DIR)/Mobility/BonnMotionMobility.o \
	$(INET_DIR)/Mobility/CircleMobility.o \
	$(INET_DIR)/Mobility/ConstSpeedMobility.o \
	$(INET_DIR)/Mobility/LinearMobility.o \
	$(INET_DIR)/Mobility/LineSegmentsMobilityBase.o \
	$(INET_DIR)/Mobility/MassMobility.o \
	$(INET_DIR)/Mobility/NullMobility.o \
	$(INET_DIR)/Mobility/RandomWPMobility.o \
	$(INET_DIR)/Mobility/RectangleMobility.o \
	$(INET_DIR)/Mobility/TurtleMobility.o \
	$(INET_DIR)/Network/ARP/ARPPacket_m.o \
	$(INET_DIR)/Network/ARP/ARP.o \
	$(INET_DIR)/Network/AutoRouting/FlatNetworkConfigurator6.o \
	$(INET_DIR)/Network/AutoRouting/FlatNetworkConfigurator.o \
	$(INET_DIR)/Network/AutoRouting/NetworkConfigurator.o \
	$(INET_DIR)/Network/Contract/IPControlInfo_m.o \
	$(INET_DIR)/Network/Contract/IPProtocolId_m.o \
	$(INET_DIR)/Network/Contract/IPv6ControlInfo_m.o \
	$(INET_DIR)/Network/Contract/InterfaceEntry.o \
	$(INET_DIR)/Network/Contract/InterfaceTable.o \
	$(INET_DIR)/Network/Contract/IPAddress.o \
	$(INET_DIR)/Network/Contract/IPAddressResolver.o \
	$(INET_DIR)/Network/Contract/IPControlInfo.o \
	$(INET_DIR)/Network/Contract/IPv6Address.o \
	$(INET_DIR)/Network/Contract/IPv6ControlInfo.o \
	$(INET_DIR)/Network/Contract/IPvXAddress.o \
	$(INET_DIR)/Network/Extras/Dummy.o \
	$(INET_DIR)/Network/Extras/FailureManager.o \
	$(INET_DIR)/Network/ICMPv6/ICMPv6Message_m.o \
	$(INET_DIR)/Network/ICMPv6/IPv6NDMessage_m.o \
	$(INET_DIR)/Network/ICMPv6/ICMPv6.o \
	$(INET_DIR)/Network/ICMPv6/IPv6NeighbourCache.o \
	$(INET_DIR)/Network/ICMPv6/IPv6NeighbourDiscovery.o \
	$(INET_DIR)/Network/IPv4/ControlManetRouting_m.o \
	$(INET_DIR)/Network/IPv4/ICMPMessage_m.o \
	$(INET_DIR)/Network/IPv4/IPDatagram_m.o \
	$(INET_DIR)/Network/IPv4/ErrorHandling.o \
	$(INET_DIR)/Network/IPv4/ICMP.o \
	$(INET_DIR)/Network/IPv4/IP.o \
	$(INET_DIR)/Network/IPv4/IPFragBuf.o \
	$(INET_DIR)/Network/IPv4/IPv4InterfaceData.o \
	$(INET_DIR)/Network/IPv4/RoutingTable.o \
	$(INET_DIR)/Network/IPv4/RoutingTableParser.o \
	$(INET_DIR)/Network/IPv6/IPv6Datagram_m.o \
	$(INET_DIR)/Network/IPv6/IPv6ExtensionHeaders_m.o \
	$(INET_DIR)/Network/IPv6/IPv6.o \
	$(INET_DIR)/Network/IPv6/IPv6Datagram.o \
	$(INET_DIR)/Network/IPv6/IPv6ErrorHandling.o \
	$(INET_DIR)/Network/IPv6/IPv6FragBuf.o \
	$(INET_DIR)/Network/IPv6/IPv6InterfaceData.o \
	$(INET_DIR)/Network/IPv6/RoutingTable6.o \
	$(INET_DIR)/Network/LDP/LDPPacket_m.o \
	$(INET_DIR)/Network/LDP/LDP.o \
	$(INET_DIR)/Network/MPLS/LIBTable.o \
	$(INET_DIR)/Network/MPLS/MPLS.o \
	$(INET_DIR)/Network/MPLS/MPLSPacket.o \
	$(INET_DIR)/Network/OSPFv2/Interface/OSPFInterface.o \
	$(INET_DIR)/Network/OSPFv2/Interface/OSPFInterfaceStateBackup.o \
	$(INET_DIR)/Network/OSPFv2/Interface/OSPFInterfaceState.o \
	$(INET_DIR)/Network/OSPFv2/Interface/OSPFInterfaceStateDesignatedRouter.o \
	$(INET_DIR)/Network/OSPFv2/Interface/OSPFInterfaceStateDown.o \
	$(INET_DIR)/Network/OSPFv2/Interface/OSPFInterfaceStateLoopback.o \
	$(INET_DIR)/Network/OSPFv2/Interface/OSPFInterfaceStateNotDesignatedRouter.o \
	$(INET_DIR)/Network/OSPFv2/Interface/OSPFInterfaceStatePointToPoint.o \
	$(INET_DIR)/Network/OSPFv2/Interface/OSPFInterfaceStateWaiting.o \
	$(INET_DIR)/Network/OSPFv2/MessageHandler/DatabaseDescriptionHandler.o \
	$(INET_DIR)/Network/OSPFv2/MessageHandler/HelloHandler.o \
	$(INET_DIR)/Network/OSPFv2/MessageHandler/LinkStateAcknowledgementHandler.o \
	$(INET_DIR)/Network/OSPFv2/MessageHandler/LinkStateRequestHandler.o \
	$(INET_DIR)/Network/OSPFv2/MessageHandler/LinkStateUpdateHandler.o \
	$(INET_DIR)/Network/OSPFv2/MessageHandler/MessageHandler.o \
	$(INET_DIR)/Network/OSPFv2/Neighbor/OSPFNeighbor.o \
	$(INET_DIR)/Network/OSPFv2/Neighbor/OSPFNeighborStateAttempt.o \
	$(INET_DIR)/Network/OSPFv2/Neighbor/OSPFNeighborState.o \
	$(INET_DIR)/Network/OSPFv2/Neighbor/OSPFNeighborStateDown.o \
	$(INET_DIR)/Network/OSPFv2/Neighbor/OSPFNeighborStateExchange.o \
	$(INET_DIR)/Network/OSPFv2/Neighbor/OSPFNeighborStateExchangeStart.o \
	$(INET_DIR)/Network/OSPFv2/Neighbor/OSPFNeighborStateFull.o \
	$(INET_DIR)/Network/OSPFv2/Neighbor/OSPFNeighborStateInit.o \
	$(INET_DIR)/Network/OSPFv2/Neighbor/OSPFNeighborStateLoading.o \
	$(INET_DIR)/Network/OSPFv2/Neighbor/OSPFNeighborStateTwoWay.o \
	$(INET_DIR)/Network/OSPFv2/Router/ASExternalLSA.o \
	$(INET_DIR)/Network/OSPFv2/Router/NetworkLSA.o \
	$(INET_DIR)/Network/OSPFv2/Router/OSPFArea.o \
	$(INET_DIR)/Network/OSPFv2/Router/OSPFRouter.o \
	$(INET_DIR)/Network/OSPFv2/Router/RouterLSA.o \
	$(INET_DIR)/Network/OSPFv2/Router/SummaryLSA.o \
	$(INET_DIR)/Network/OSPFv2/OSPFPacket_m.o \
	$(INET_DIR)/Network/OSPFv2/OSPFTimer_m.o \
	$(INET_DIR)/Network/OSPFv2/OSPFRouting.o \
	$(INET_DIR)/Network/Quagga/Daemon.o \
	$(INET_DIR)/Network/Quagga/glue.o \
	$(INET_DIR)/Network/Quagga/Netlink.o \
	$(INET_DIR)/Network/Quagga/oppsim_kernel.o \
	$(INET_DIR)/Network/Quagga/RawSocket.o \
	$(INET_DIR)/Network/Quagga/SocketMsg_m.o \
	$(INET_DIR)/Network/Quagga/SocketMsg.o \
	$(INET_DIR)/Network/Queue/BasicDSCPClassifier.o \
	$(INET_DIR)/Network/Queue/DropTailQoSQueue.o \
	$(INET_DIR)/Network/Queue/DropTailQueue.o \
	$(INET_DIR)/Network/Queue/REDQueue.o \
	$(INET_DIR)/Network/RSVP_TE/IntServ_m.o \
	$(INET_DIR)/Network/RSVP_TE/RSVPHello_m.o \
	$(INET_DIR)/Network/RSVP_TE/RSVPPacket_m.o \
	$(INET_DIR)/Network/RSVP_TE/RSVPPathMsg_m.o \
	$(INET_DIR)/Network/RSVP_TE/RSVPResvMsg_m.o \
	$(INET_DIR)/Network/RSVP_TE/SignallingMsg_m.o \
	$(INET_DIR)/Network/RSVP_TE/RSVP.o \
	$(INET_DIR)/Network/RSVP_TE/SimpleClassifier.o \
	$(INET_DIR)/Network/RSVP_TE/Utils.o \
	$(INET_DIR)/Network/TED/LinkStatePacket_m.o \
	$(INET_DIR)/Network/TED/TED_m.o \
	$(INET_DIR)/Network/TED/LinkStateRouting.o \
	$(INET_DIR)/Network/TED/TED.o \
	$(INET_DIR)/NetworkInterfaces/Contract/Ieee802Ctrl_m.o \
	$(INET_DIR)/NetworkInterfaces/Contract/PhyControlInfo_m.o \
	$(INET_DIR)/NetworkInterfaces/Contract/MACAddress.o \
	$(INET_DIR)/NetworkInterfaces/EtherSwitch/MACRelayUnitBase.o \
	$(INET_DIR)/NetworkInterfaces/EtherSwitch/MACRelayUnitNP.o \
	$(INET_DIR)/NetworkInterfaces/EtherSwitch/MACRelayUnitPP.o \
	$(INET_DIR)/NetworkInterfaces/Ethernet/EtherFrame_m.o \
	$(INET_DIR)/NetworkInterfaces/Ethernet/EtherBus.o \
	$(INET_DIR)/NetworkInterfaces/Ethernet/EtherEncap.o \
	$(INET_DIR)/NetworkInterfaces/Ethernet/EtherHub.o \
	$(INET_DIR)/NetworkInterfaces/Ethernet/EtherLLC.o \
	$(INET_DIR)/NetworkInterfaces/Ethernet/EtherMAC2.o \
	$(INET_DIR)/NetworkInterfaces/Ethernet/EtherMACBase.o \
	$(INET_DIR)/NetworkInterfaces/Ethernet/EtherMAC.o \
	$(INET_DIR)/NetworkInterfaces/Ethernet/utils.o \
	$(INET_DIR)/NetworkInterfaces/Ieee80211/Mac/Ieee80211Frame_m.o \
	$(INET_DIR)/NetworkInterfaces/Ieee80211/Mac/Ieee80211Mac.o \
	$(INET_DIR)/NetworkInterfaces/Ieee80211/Mgmt/Ieee80211MgmtFrames_m.o \
	$(INET_DIR)/NetworkInterfaces/Ieee80211/Mgmt/Ieee80211Primitives_m.o \
	$(INET_DIR)/NetworkInterfaces/Ieee80211/Mgmt/Ieee80211AgentSTA.o \
	$(INET_DIR)/NetworkInterfaces/Ieee80211/Mgmt/Ieee80211MgmtAdhoc.o \
	$(INET_DIR)/NetworkInterfaces/Ieee80211/Mgmt/Ieee80211MgmtAPBase.o \
	$(INET_DIR)/NetworkInterfaces/Ieee80211/Mgmt/Ieee80211MgmtAP.o \
	$(INET_DIR)/NetworkInterfaces/Ieee80211/Mgmt/Ieee80211MgmtAPSimplified.o \
	$(INET_DIR)/NetworkInterfaces/Ieee80211/Mgmt/Ieee80211MgmtBase.o \
	$(INET_DIR)/NetworkInterfaces/Ieee80211/Mgmt/Ieee80211MgmtSTA.o \
	$(INET_DIR)/NetworkInterfaces/Ieee80211/Mgmt/Ieee80211MgmtSTASimplified.o \
	$(INET_DIR)/NetworkInterfaces/MF80211/macLayer/Mac80211Pkt_m.o \
	$(INET_DIR)/NetworkInterfaces/MF80211/macLayer/MacPkt_m.o \
	$(INET_DIR)/NetworkInterfaces/MF80211/macLayer/CSMAMacLayer.o \
	$(INET_DIR)/NetworkInterfaces/MF80211/macLayer/Mac80211.o \
	$(INET_DIR)/NetworkInterfaces/MF80211/phyLayer/decider/Decider80211.o \
	$(INET_DIR)/NetworkInterfaces/MF80211/phyLayer/decider/ErrAndCollDecider.o \
	$(INET_DIR)/NetworkInterfaces/MF80211/phyLayer/decider/SnrDecider.o \
	$(INET_DIR)/NetworkInterfaces/MF80211/phyLayer/snrEval/GilbertElliotSnr.o \
	$(INET_DIR)/NetworkInterfaces/MF80211/phyLayer/snrEval/SnrEval80211.o \
	$(INET_DIR)/NetworkInterfaces/MF80211/phyLayer/snrEval/SnrEval.o \
	$(INET_DIR)/NetworkInterfaces/MFCore/AirFrame_m.o \
	$(INET_DIR)/NetworkInterfaces/MFCore/SnrControlInfo_m.o \
	$(INET_DIR)/NetworkInterfaces/MFCore/TransmComplete_m.o \
	$(INET_DIR)/NetworkInterfaces/MFCore/BasicDecider.o \
	$(INET_DIR)/NetworkInterfaces/MFCore/BasicSnrEval.o \
	$(INET_DIR)/NetworkInterfaces/MFCore/WirelessMacBase.o \
	$(INET_DIR)/NetworkInterfaces/PPP/PPPFrame_m.o \
	$(INET_DIR)/NetworkInterfaces/PPP/PPP.o \
	$(INET_DIR)/NetworkInterfaces/PPP/ThruputMeter.o \
	$(INET_DIR)/NetworkInterfaces/Radio/AbstractRadio.o \
	$(INET_DIR)/NetworkInterfaces/Radio/GenericRadio.o \
	$(INET_DIR)/NetworkInterfaces/Radio/GenericRadioModel.o \
	$(INET_DIR)/NetworkInterfaces/Radio/Ieee80211Radio.o \
	$(INET_DIR)/NetworkInterfaces/Radio/Ieee80211RadioModel.o \
	$(INET_DIR)/NetworkInterfaces/Radio/Modulation.o \
	$(INET_DIR)/NetworkInterfaces/Radio/PathLossReceptionModel.o \
	$(INET_DIR)/Transport/Contract/TCPCommand_m.o \
	$(INET_DIR)/Transport/Contract/UDPControlInfo_m.o \
	$(INET_DIR)/Transport/Contract/TCPSocket.o \
	$(INET_DIR)/Transport/Contract/TCPSocketMap.o \
	$(INET_DIR)/Transport/Contract/UDPSocket.o \
	$(INET_DIR)/Transport/TCP/flavours/DumbTCP.o \
	$(INET_DIR)/Transport/TCP/flavours/TCPBaseAlg.o \
	$(INET_DIR)/Transport/TCP/flavours/TCPNoCongestionControl.o \
	$(INET_DIR)/Transport/TCP/flavours/TCPReno.o \
	$(INET_DIR)/Transport/TCP/flavours/TCPTahoe.o \
	$(INET_DIR)/Transport/TCP/flavours/TCPTahoeRenoFamily.o \
	$(INET_DIR)/Transport/TCP/queues/TCPMsgBasedRcvQueue.o \
	$(INET_DIR)/Transport/TCP/queues/TCPMsgBasedSendQueue.o \
	$(INET_DIR)/Transport/TCP/queues/TCPVirtualDataRcvQueue.o \
	$(INET_DIR)/Transport/TCP/queues/TCPVirtualDataSendQueue.o \
	$(INET_DIR)/Transport/TCP/TCPSegment_m.o \
	$(INET_DIR)/Transport/TCP/TCP.o \
	$(INET_DIR)/Transport/TCP/TCPConnectionBase.o \
	$(INET_DIR)/Transport/TCP/TCPConnectionEventProc.o \
	$(INET_DIR)/Transport/TCP/TCPConnectionRcvSegment.o \
	$(INET_DIR)/Transport/TCP/TCPConnectionUtil.o \
	$(INET_DIR)/Transport/TCP/TCPSegment.o \
	$(INET_DIR)/Transport/UDP/UDPPacket_m.o \
	$(INET_DIR)/Transport/UDP/UDP.o \
	$(INET_DIR)/Util/HeaderSerializers/ICMPSerializer.o \
	$(INET_DIR)/Util/HeaderSerializers/IPSerializer.o \
	$(INET_DIR)/Util/HeaderSerializers/UDPSerializer.o \
	$(INET_DIR)/Util/common.o \
	$(INET_DIR)/Util/NAMTraceWriter.o \
	$(INET_DIR)/Util/opp_utils.o \
	$(INET_DIR)/Util/TCPDump.o \
	$(INET_DIR)/Util/ThruputMeteringChannel.o \
	$(INET_DIR)/Util/XMLUtils.o \
	$(INET_DIR)/World/ChannelAccess.o \
	$(INET_DIR)/World/ChannelControl.o \
	$(INET_DIR)/World/ChannelInstaller.o \
	$(INET_DIR)/World/NAMTrace.o \
	$(INET_DIR)/World/ScenarioManager.o \
	$(INET_DIR)/Network/Quagga/quaggasrc/quagga/globalvars.o


$(INET_DIR)/bin/INET:
	cd INET && ./makemake
	cd INET && make ROOT=$(shell pwd)/$(INET_DIR)

#
# A little hack to get the inet objects to build first
#
$(INET_OBJS): $(INET_DIR)/bin/INET

#
# Archives created by and for the INET framework package
#
lib/inet.a: $(INET_DIR)/bin/INET
	mkdir -p lib
	$(AR) rcs $@ $(INET_OBJS)

lib/ospfd.a: $(INET_DIR)/bin/INET
	mkdir -p lib
	$(CP) $(INET_DIR)/Network/Quagga/quaggasrc/quagga/ospfd/ospfd.a $@

lib/ripd.a: $(INET_DIR)/bin/INET
	mkdir -p lib
	$(CP) $(INET_DIR)/Network/Quagga/quaggasrc/quagga/ripd/ripd.a $@

lib/zebra.a: $(INET_DIR)/bin/INET
	mkdir -p lib
	$(CP) $(INET_DIR)/Network/Quagga/quaggasrc/quagga/zebra/zebra.a $@

lib/libzebra.a: $(INET_DIR)/bin/INET
	mkdir -p lib
	$(CP) $(INET_DIR)/Network/Quagga/quaggasrc/quagga/lib/libzebra.a $@

#
# Clean out INET build
#
inet_clean:
	cd INET && make ROOT=$(shell pwd)/$(INET_DIR) clean
	$(RM) lib/inet.a lib/ospfd.a lib/ripd.a lib/zebra.a lib/libzebra.a

.PHONY: inet_clean
