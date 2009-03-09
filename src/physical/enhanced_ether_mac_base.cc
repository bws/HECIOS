//
// Copyright (C) 2006 Levente Meszaros
// Copyright (C) 2004 Andras Varga
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
//
#include <cassert>
#include <stdio.h>
#include <string.h>
#include <omnetpp.h>
#include "enhanced_ether_mac_base.h"
#include "IPassiveQueue.h"
#include "InterfaceTable.h"
#include "InterfaceTableAccess.h"
using namespace std;

EnhancedEtherMACBase::EnhancedEtherMACBase()
{
    nb = NULL;
    queueModule = NULL;
    interfaceEntry = NULL;
    endTxMsg = endIFGMsg = endPauseMsg = NULL;
}

EnhancedEtherMACBase::~EnhancedEtherMACBase()
{
    cancelAndDelete(endTxMsg);
    cancelAndDelete(endIFGMsg);
    cancelAndDelete(endPauseMsg);
}

void EnhancedEtherMACBase::initialize()
{
    initializeFlags();

    initializeTxrate();
    WATCH(txrate);

    initializeMACAddress();
    initializeQueueModule();
    initializeNotificationBoard();
    initializeStatistics();

    // Retrieve the MTU
    mtuSize = par("mtu");

    registerInterface(txrate); // needs MAC address

    // initialize queue
    txQueue.setName("txQueue");

    // initialize self messages
    endTxMsg = new cMessage("EndTransmission", ENDTRANSMISSION);
    endIFGMsg = new cMessage("EndIFG", ENDIFG);
    endPauseMsg = new cMessage("EndPause", ENDPAUSE);

    // initialize states
    transmitState = TX_IDLE_STATE;
    receiveState = RX_IDLE_STATE;
    WATCH(transmitState);
    WATCH(receiveState);

    // initalize pause
    pauseUnitsRequested = 0;
    WATCH(pauseUnitsRequested);

    // initialize queue limit
    txQueueLimit = par("txQueueLimit");
    WATCH(txQueueLimit);

    // Cache the gate ids
    physInGateId = findGate("physIn");
    physOutGateId = findGate("physOut");
    upperLayerInGateId = findGate("upperLayerIn");
    upperLayerOutGateId = findGate("upperLayerOut");
}

void EnhancedEtherMACBase::initializeQueueModule()
{
    if (par("queueModule").stringValue()[0])
    {
        cModule *module = parentModule()->submodule(par("queueModule").stringValue());
        queueModule = check_and_cast<IPassiveQueue *>(module);
        EV << "Requesting first frame from queue module\n";
        queueModule->requestPacket();
    }
}

void EnhancedEtherMACBase::initializeMACAddress()
{
    const char *addrstr = par("address");

    if (!strcmp(addrstr,"auto"))
    {
        // assign automatic address
        address = MACAddress::generateAutoAddress();

        // change module parameter from "auto" to concrete address
        par("address").setStringValue(address.str().c_str());
    }
    else
    {
        address.setAddress(addrstr);
    }
}

void EnhancedEtherMACBase::initializeNotificationBoard()
{
    if (interfaceEntry) {
        nb = NotificationBoardAccess().getIfExists();
        notifDetails.setInterfaceEntry(interfaceEntry);
    }
}

void EnhancedEtherMACBase::initializeFlags()
{
    // initialize connected flag
    connected = gate("physOut")->destinationGate()->isConnected();
    if (!connected) EV << "MAC not connected to a network.\n";
    WATCH(connected);

    // TODO: this should be settable from the gui
    // initialize disabled flag
    // Note: it is currently not supported to enable a disabled MAC at runtime.
    // Difficulties: (1) autoconfig (2) how to pick up channel state (free, tx, collision etc)
    disabled = false;
    WATCH(disabled);

    // initialize promiscuous flag
    promiscuous = par("promiscuous");
    WATCH(promiscuous);
}

void EnhancedEtherMACBase::initializeStatistics()
{
    framesSentInBurst = 0;
    bytesSentInBurst = 0;

    numFramesSent = numFramesReceivedOK = numBytesSent = numBytesReceivedOK = 0;
    numFramesPassedToHL = numDroppedBitError = numDroppedNotForUs = 0;
    numFramesFromHL = numDroppedIfaceDown = 0;
    numPauseFramesRcvd = numPauseFramesSent = 0;

    WATCH(framesSentInBurst);
    WATCH(bytesSentInBurst);

    WATCH(numFramesSent);
    WATCH(numFramesReceivedOK);
    WATCH(numBytesSent);
    WATCH(numBytesReceivedOK);
    WATCH(numFramesFromHL);
    WATCH(numDroppedIfaceDown);
    WATCH(numDroppedBitError);
    WATCH(numDroppedNotForUs);
    WATCH(numFramesPassedToHL);
    WATCH(numPauseFramesRcvd);
    WATCH(numPauseFramesSent);

    numFramesSentVector.setName("framesSent");
    numFramesReceivedOKVector.setName("framesReceivedOK");
    numBytesSentVector.setName("bytesSent");
    numBytesReceivedOKVector.setName("bytesReceivedOK");
    numDroppedIfaceDownVector.setName("framesDroppedIfaceDown");
    numDroppedBitErrorVector.setName("framesDroppedBitError");
    numDroppedNotForUsVector.setName("framesDroppedNotForUs");
    numFramesPassedToHLVector.setName("framesPassedToHL");
    numPauseFramesRcvdVector.setName("pauseFramesRcvd");
    numPauseFramesSentVector.setName("pauseFramesSent");
}

void EnhancedEtherMACBase::registerInterface(double txrate)
{
    InterfaceTable *ift = InterfaceTableAccess().getIfExists();
    if (!ift)
        return;

    interfaceEntry = new InterfaceEntry();

    // interface name: our module name without special characters ([])
    char *interfaceName = new char[strlen(parentModule()->fullName())+1];
    char *d=interfaceName;
    for (const char *s=parentModule()->fullName(); *s; s++)
        if (isalnum(*s))
            *d++ = *s;
    *d = '\0';

    interfaceEntry->setName(interfaceName);
    delete [] interfaceName;

    // data rate
    interfaceEntry->setDatarate(txrate);

    // generate a link-layer address to be used as interface token for IPv6
    interfaceEntry->setMACAddress(address);
    interfaceEntry->setInterfaceToken(address.formInterfaceIdentifier());
    //InterfaceToken token(0, simulation.getUniqueNumber(), 64);
    //interfaceEntry->setInterfaceToken(token);

    // MTU: typical values are 576 (Internet de facto), 1500 (Ethernet-friendly),
    // 4000 (on some point-to-point links), 4470 (Cisco routers default, FDDI compatible)
    interfaceEntry->setMtu(mtuSize);

    // capabilities
    interfaceEntry->setMulticast(true);
    interfaceEntry->setBroadcast(true);

    // add
    ift->addInterface(interfaceEntry, this);
}


bool EnhancedEtherMACBase::checkDestinationAddress(EtherFrame *frame)
{
    // If not set to promiscuous = on, then checks if received frame contains destination MAC address
    // matching port's MAC address, also checks if broadcast bit is set
    if (!promiscuous && !frame->getDest().isBroadcast() && !frame->getDest().equals(address))
    {
        EV << "Frame `" << frame->name() <<"' not destined to us, discarding\n";
        numDroppedNotForUs++;
        numDroppedNotForUsVector.record(numDroppedNotForUs);
        delete frame;

        return false;
    }

    return true;
}

void EnhancedEtherMACBase::calculateParameters()
{
    assert(true == duplexMode);

    if (disabled || !connected)
    {
        bitTime = slotTime = interFrameGap = jamDuration = shortestFrameDuration = 0;
        carrierExtension = frameBursting = false;
        return;
    }

    // BWS
    // Modifications made to allow channel rates other than just ethernet
    // conforming channel rates
    // BWS
    if (txrate != ETHERNET_TXRATE && txrate != FAST_ETHERNET_TXRATE &&
        txrate != GIGABIT_ETHERNET_TXRATE && txrate != FAST_GIGABIT_ETHERNET_TXRATE)
    {
        EV << "Using non-standard ethernet transmission rate: "
           << txrate << "bit/sec\n";
    }

    bitTime = 1/(double)txrate;

    // set slot time
    if (txrate <= FAST_ETHERNET_TXRATE)
        slotTime = SLOT_TIME;
    else
        slotTime = GIGABIT_SLOT_TIME;

    // only if Gigabit Ethernet or better
    frameBursting = (txrate >= GIGABIT_ETHERNET_TXRATE);
    carrierExtension = (slotTime == GIGABIT_SLOT_TIME && !duplexMode);

    interFrameGap = INTERFRAME_GAP_BITS/(double)txrate;
    jamDuration = 8*JAM_SIGNAL_BYTES*bitTime;
    shortestFrameDuration = carrierExtension ? GIGABIT_MIN_FRAME_WITH_EXT : MIN_ETHERNET_FRAME;

    if (txrate > GIGABIT_ETHERNET_TXRATE)
    {
        interFrameGap = 10.0 / (double)txrate;
        shortestFrameDuration = 0.0;
    }
}

void EnhancedEtherMACBase::printParameters()
{
    // Dump parameters
    EV << "MAC address: " << address << (promiscuous ? ", promiscuous mode" : "") << endl;
    EV << "txrate: " << txrate << ", " << (duplexMode ? "full duplex" : "half-duplex") << endl;
    EV << "bitTime: " << bitTime << endl;
    EV << "carrierExtension: " << carrierExtension << endl;
    EV << "frameBursting: " << frameBursting << endl;
    EV << "slotTime: " << slotTime << endl;
    EV << "interFrameGap: " << interFrameGap << endl;
    EV << endl;
}

void EnhancedEtherMACBase::processFrameFromUpperLayer(EtherFrame *frame)
{
    //EV << "Received frame from upper layer: " << frame << endl;

    // check message kind
    if (frame->kind()!=ETH_FRAME && frame->kind()!=ETH_PAUSE)
        error("message with unexpected message kind %d arrived from higher layer", frame->kind());

    // pause frames must be EtherPauseFrame AND kind==ETH_PAUSE
    ASSERT((frame->kind()==ETH_PAUSE) == (dynamic_cast<EtherPauseFrame *>(frame)!=NULL));

    if (frame->getDest().equals(address))
    {
        error("logic error: frame %s from higher layer has local MAC address as dest (%s)",
              frame->fullName(), frame->getDest().str().c_str());
    }

    if (frame->byteLength() > MAX_ETHERNET_FRAME_SIZE)
        error("packet from higher layer (%d bytes) exceeds maximum Ethernet frame size (%d)", frame->byteLength(), MAX_ETHERNET_FRAME);

    // must be ETH_FRAME (or ETH_PAUSE) from upper layer
    bool isPauseFrame = (frame->kind()==ETH_PAUSE);
    if (!isPauseFrame)
    {
        numFramesFromHL++;

        if (txQueueLimit && txQueue.length()>txQueueLimit)
            error("txQueue length exceeds %d -- this is probably due to "
                  "a bogus app model generating excessive traffic "
                  "(or if this is normal, increase txQueueLimit!)",
                  txQueueLimit);

        // fill in src address if not set
        if (frame->getSrc().isUnspecified())
            frame->setSrc(address);

        // store frame and possibly begin transmitting
        //EV << "Packet " << frame << " arrived from higher layers, enqueueing\n";
        txQueue.insert(frame);
    }
    else
    {
        //EV << "PAUSE received from higher layer\n";

        // PAUSE frames enjoy priority -- they're transmitted before all other frames queued up
        if (!txQueue.empty())
            txQueue.insertBefore(txQueue.tail(), frame);  // tail() frame is probably being transmitted
        else
            txQueue.insert(frame);
    }

}

void EnhancedEtherMACBase::processMsgFromNetwork(cMessage *frame)
{
    //EV << "Received frame from network: " << frame << endl;

    // frame must be ETH_FRAME, ETH_PAUSE or JAM_SIGNAL
    if (frame->kind()!=ETH_FRAME && frame->kind()!=ETH_PAUSE && frame->kind()!=JAM_SIGNAL)
        error("message with unexpected message kind %d arrived from network", frame->kind());

    // detect cable length violation in half-duplex mode
    if (!duplexMode && simTime()-frame->sendingTime()>=shortestFrameDuration)
        error("very long frame propagation time detected, maybe cable exceeds maximum allowed length? "
              "(%lgs corresponds to an approx. %lgm cable)",
              simTime()-frame->sendingTime(),
              (simTime()-frame->sendingTime())*200000000.0);
}

void EnhancedEtherMACBase::frameReceptionComplete(EtherFrame *frame)
{
    int pauseUnits;

    switch (frame->kind())
    {
      case ETH_FRAME:
        processReceivedDataFrame((EtherFrame *)frame);
        break;

      case ETH_PAUSE:
        pauseUnits = ((EtherPauseFrame *)frame)->getPauseTime();
        delete frame;
        numPauseFramesRcvd++;
        numPauseFramesRcvdVector.record(numPauseFramesRcvd);
        processPauseCommand(pauseUnits);
        break;

      default:
        error("Invalid message kind %d",frame->kind());
    }
}

void EnhancedEtherMACBase::processReceivedDataFrame(EtherFrame *frame)
{
    // bit errors
    if (frame->hasBitError())
    {
        numDroppedBitError++;
        numDroppedBitErrorVector.record(numDroppedBitError);
        delete frame;
        return;
    }

    // strip preamble and SFD
    frame->addByteLength(-PREAMBLE_BYTES-SFD_BYTES);

    // statistics
    numFramesReceivedOK++;
    numBytesReceivedOK += frame->byteLength();
    numFramesReceivedOKVector.record(numFramesReceivedOK);
    numBytesReceivedOKVector.record(numBytesReceivedOK);

    if (!checkDestinationAddress(frame))
        return;

    numFramesPassedToHL++;
    numFramesPassedToHLVector.record(numFramesPassedToHL);

    // pass up to upper layer
    send(frame, upperLayerOutGateId);
}

void EnhancedEtherMACBase::processPauseCommand(int pauseUnits)
{
    if (transmitState==TX_IDLE_STATE)
    {
        EV << "PAUSE frame received, pausing for " << pauseUnitsRequested << " time units\n";
        if (pauseUnits>0)
            scheduleEndPausePeriod(pauseUnits);
    }
    else if (transmitState==PAUSE_STATE)
    {
        EV << "PAUSE frame received, pausing for " << pauseUnitsRequested << " more time units from now\n";
        cancelEvent(endPauseMsg);
        if (pauseUnits>0)
            scheduleEndPausePeriod(pauseUnits);
    }
    else
    {
        // transmitter busy -- wait until it finishes with current frame (endTx)
        // and then it'll go to PAUSE state
        EV << "PAUSE frame received, storing pause request\n";
        pauseUnitsRequested = pauseUnits;
    }
}

void EnhancedEtherMACBase::handleEndIFGPeriod()
{
    if (transmitState!=WAIT_IFG_STATE)
        error("Not in WAIT_IFG_STATE at the end of IFG period");

    if (txQueue.empty())
        error("End of IFG and no frame to transmit");

    // End of IFG period, okay to transmit, if Rx idle OR duplexMode
    cMessage *frame = (cMessage *)txQueue.tail();
    EV << "IFG elapsed, now begin transmission of frame " << frame << endl;

    // Perform carrier extension if in Gigabit Ethernet
    if (carrierExtension && frame->byteLength() < GIGABIT_MIN_FRAME_WITH_EXT)
    {
        EV << "Performing carrier extension of small frame\n";
        frame->setByteLength(GIGABIT_MIN_FRAME_WITH_EXT);
    }

    // start frame burst, if enabled
    if (frameBursting)
    {
        EV << "Starting frame burst\n";
        framesSentInBurst = 0;
        bytesSentInBurst = 0;
    }
}

void EnhancedEtherMACBase::handleEndTxPeriod()
{
    // we only get here if transmission has finished successfully, without collision
    if (transmitState!=TRANSMITTING_STATE || (!duplexMode && receiveState!=RX_IDLE_STATE))
        error("End of transmission, and incorrect state detected");

    if (txQueue.empty())
        error("Frame under transmission cannot be found");

    // get frame from buffer
    cMessage *frame = (cMessage*)txQueue.pop();

    numFramesSent++;
    numBytesSent += frame->byteLength();
    numFramesSentVector.record(numFramesSent);
    numBytesSentVector.record(numBytesSent);

    if (frame->kind()==ETH_PAUSE)
    {
        numPauseFramesSent++;
        numPauseFramesSentVector.record(numPauseFramesSent);
    }

    EV << "Transmission of " << frame << " successfully completed\n";
    delete frame;
}

void EnhancedEtherMACBase::handleEndPausePeriod()
{
    if (transmitState != PAUSE_STATE)
        error("At end of PAUSE not in PAUSE_STATE!");
    EV << "Pause finished, resuming transmissions\n";
    beginSendFrames();
}

void EnhancedEtherMACBase::processMessageWhenNotConnected(cMessage *msg)
{
    EV << "Interface is not connected -- dropping packet " << msg << endl;
    delete msg;
    numDroppedIfaceDown++;
}

void EnhancedEtherMACBase::processMessageWhenDisabled(cMessage *msg)
{
    EV << "MAC is disabled -- dropping message " << msg << endl;
    delete msg;
}

void EnhancedEtherMACBase::scheduleEndIFGPeriod()
{
    scheduleAt(simTime()+interFrameGap, endIFGMsg);
    transmitState = WAIT_IFG_STATE;
}

void EnhancedEtherMACBase::scheduleEndTxPeriod(cMessage *frame)
{
    scheduleAt(simTime()+frame->length()*bitTime, endTxMsg);
    transmitState = TRANSMITTING_STATE;
}

void EnhancedEtherMACBase::scheduleEndPausePeriod(int pauseUnits)
{
    // length is interpreted as 512-bit-time units
    double pausePeriod = pauseUnits*PAUSE_BITTIME*bitTime;
    scheduleAt(simTime()+pausePeriod, endPauseMsg);
    transmitState = PAUSE_STATE;
}

bool EnhancedEtherMACBase::checkAndScheduleEndPausePeriod()
{
    if (pauseUnitsRequested>0)
    {
        // if we received a PAUSE frame recently, go into PAUSE state
        EV << "Going to PAUSE mode for " << pauseUnitsRequested << " time units\n";

        scheduleEndPausePeriod(pauseUnitsRequested);
        pauseUnitsRequested = 0;
        return true;
    }

    return false;
}

void EnhancedEtherMACBase::beginSendFrames()
{
    if (!txQueue.empty())
    {
        // Other frames are queued, therefore wait IFG period and transmit next frame
        EV << "Transmit next frame in output queue, after IFG period\n";
        scheduleEndIFGPeriod();
    }
    else
    {
        transmitState = TX_IDLE_STATE;
        if (queueModule)
        {
            // tell queue module that we've become idle
            EV << "Requesting another frame from queue module\n";
            queueModule->requestPacket();
        }
        else
        {
            // No more frames set transmitter to idle
            EV << "No more frames to send, transmitter set to idle\n";
        }
    }
}

void EnhancedEtherMACBase::fireChangeNotification(int type, cMessage *msg)
{
   	if (nb) {
	    notifDetails.setMessage(msg);
		nb->fireChangeNotification(type, &notifDetails);
	}
}

void EnhancedEtherMACBase::finish()
{
    if (!disabled && par("writeScalars").boolValue())
    {
        double t = simTime();
        recordScalar("simulated time", t);
        recordScalar("txrate (Mb)", txrate/1000000);
        recordScalar("full duplex", duplexMode);
        recordScalar("frames sent",    numFramesSent);
        recordScalar("frames rcvd",    numFramesReceivedOK);
        recordScalar("bytes sent",     numBytesSent);
        recordScalar("bytes rcvd",     numBytesReceivedOK);
        recordScalar("frames from higher layer", numFramesFromHL);
        recordScalar("frames from higher layer dropped (iface down)", numDroppedIfaceDown);
        recordScalar("frames dropped (bit error)",  numDroppedBitError);
        recordScalar("frames dropped (not for us)", numDroppedNotForUs);
        recordScalar("frames passed up to HL", numFramesPassedToHL);
        recordScalar("PAUSE frames sent",  numPauseFramesSent);
        recordScalar("PAUSE frames rcvd",  numPauseFramesRcvd);

        if (t>0)
        {
            recordScalar("frames/sec sent", numFramesSent/t);
            recordScalar("frames/sec rcvd", numFramesReceivedOK/t);
            recordScalar("bits/sec sent",   8*numBytesSent/t);
            recordScalar("bits/sec rcvd",   8*numBytesReceivedOK/t);
        }
    }
}

void EnhancedEtherMACBase::updateDisplayString()
{
    // icon coloring
    const char *color;
    if (receiveState==RX_COLLISION_STATE)
        color = "red";
    else if (transmitState==TRANSMITTING_STATE)
        color = "yellow";
    else if (transmitState==JAMMING_STATE)
        color = "red";
    else if (receiveState==RECEIVING_STATE)
        color = "#4040ff";
    else if (transmitState==BACKOFF_STATE)
        color = "white";
    else if (transmitState==PAUSE_STATE)
        color = "gray";
    else
        color = "";
    displayString().setTagArg("i",1,color);
    if (!strcmp(parentModule()->className(),"EthernetInterface"))
        parentModule()->displayString().setTagArg("i",1,color);

    // connection coloring
    updateConnectionColor(transmitState);

#if 0
    // this code works but didn't turn out to be very useful
    const char *txStateName;
    switch (transmitState) {
        case TX_IDLE_STATE:      txStateName="IDLE"; break;
        case WAIT_IFG_STATE:     txStateName="WAIT_IFG"; break;
        case TRANSMITTING_STATE: txStateName="TX"; break;
        case JAMMING_STATE:      txStateName="JAM"; break;
        case BACKOFF_STATE:      txStateName="BACKOFF"; break;
        case PAUSE_STATE:        txStateName="PAUSE"; break;
        default: error("wrong tx state");
    }
    const char *rxStateName;
    switch (receiveState) {
        case RX_IDLE_STATE:      rxStateName="IDLE"; break;
        case RECEIVING_STATE:    rxStateName="RX"; break;
        case RX_COLLISION_STATE: rxStateName="COLL"; break;
        default: error("wrong rx state");
    }

    char buf[80];
    sprintf(buf, "tx:%s rx: %s\n#boff:%d #cTx:%d",
                 txStateName, rxStateName, backoffs, numConcurrentTransmissions);
    displayString().setTagArg("t",0,buf);
#endif
}

void EnhancedEtherMACBase::updateConnectionColor(int txState)
{
    const char *color;
    if (txState==TRANSMITTING_STATE)
        color = "yellow";
    else if (txState==JAMMING_STATE || txState==BACKOFF_STATE)
        color = "red";
    else
        color = "";

    cGate *g = gate("physOut");
    while (g && g->type()=='O')
    {
        g->displayString().setTagArg("o",0,color);
        g->displayString().setTagArg("o",1, color[0] ? "3" : "1");
        g = g->toGate();
    }
}