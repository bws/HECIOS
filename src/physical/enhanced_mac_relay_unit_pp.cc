/*
 * Copyright (C) 2003 CTIE, Monash University
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

#ifdef _MSC_VER
#pragma warning(disable:4786)
#endif


#include "enhanced_mac_relay_unit_pp.h"
#include <cassert>
#include <iostream>
#include "EtherFrame_m.h"
#include "utils.h"
#include "Ethernet.h"
#include "MACAddress.h"
using namespace std;


Define_Module(EnhancedMACRelayUnitPP);


/* unused for now
static std::ostream& operator<< (std::ostream& os, cMessage *msg)
{
    os << "(" << msg->className() << ")" << msg->fullName();
    return os;
}
*/

EnhancedMACRelayUnitPP::EnhancedMACRelayUnitPP()
{
    buffer = NULL;
}

EnhancedMACRelayUnitPP::~EnhancedMACRelayUnitPP()
{
    delete [] buffer;
}

int EnhancedMACRelayUnitPP::numInitStages() const
{
    return 2;
}

void EnhancedMACRelayUnitPP::initialize(int stage)
{
    if (1 == stage)
    {
        MACRelayUnitBase::initialize();

        // Determine the number of switch ports in use
        size_t numPorts = getNumActivePorts();
        size_t bufferPerPort = par("bufferSizePerPort");
        size_t minBufferSize = par("minBufferSize");
        size_t maxBufferSize = par("maxBufferSize");
        assert (minBufferSize <= maxBufferSize);

        bufferSize = numPorts * bufferPerPort;
        if (minBufferSize > 0 && bufferSize < minBufferSize)
        {
            bufferSize = minBufferSize;
        }
        else if (maxBufferSize > 0 && bufferSize > maxBufferSize)
        {
            bufferSize = maxBufferSize;
        }

        // Configure the watermark settings
        //size_t watermarkPerPort = par("watermarkSizePerPort");
        //size_t minWatermarkSize = par("minWatermarkSize");
        highWatermark = par("highWatermark");


        bufferLevel.setName("buffer level");

        numProcessedFrames = numDroppedFrames = 0;
        WATCH(numProcessedFrames);
        WATCH(numDroppedFrames);

        processingTime = par("processingTime");
        highWatermark = par("highWatermark");
        pauseUnits = par("pauseUnits");

        // 1 pause unit is 512 bit times; we assume 100Mb MACs here.
        // We send a pause again when previous one is about to expire.
        pauseInterval = pauseUnits*512.0/100000.0;

        pauseLastSent = 0;
        WATCH(pauseLastSent);

        bufferUsed = 0;
        WATCH(bufferUsed);

        buffer = new PortBuffer[numPorts];
        for (int i = 0; i < numPorts; ++i)
        {
            buffer[i].port = i;
            buffer[i].cpuBusy = false;

            char qname[20];
            sprintf(qname,"portQueue%d",i);
            buffer[i].queue.setName(qname);
        }

        EV << "Parameters of (" << className() << ") " << fullPath() << "\n";
        EV << "processing time: " << processingTime << "\n";
        EV << "ports: " << numPorts << "\n";
        EV << "buffer size: " << bufferSize << "\n";
        EV << "address table size: " << addressTableSize << "\n";
        EV << "aging time: " << agingTime << "\n";
        EV << "high watermark: " << highWatermark << "\n";
        EV << "pause time: " << pauseUnits << "\n";
        EV << "\n";

        cerr << __FILE__ << ":" << __LINE__ << ":"
             << "DIAGNOSTIC: Switch buffer size set to: " << bufferSize << endl;
    }
}

void EnhancedMACRelayUnitPP::handleMessage(cMessage *msg)
{
    if (!msg->isSelfMessage())
    {
        // Frame received from MAC unit
        handleIncomingFrame(check_and_cast<EtherFrame *>(msg));
    }
    else
    {
        // Self message signal used to indicate a frame has been finished processing
        processFrame(msg);
    }
}

void EnhancedMACRelayUnitPP::handleIncomingFrame(EtherFrame *frame)
{
    // If buffer not full, insert payload frame into buffer and process the frame in parallel.

    long length = frame->byteLength();
    if (length + bufferUsed < bufferSize)
    {
        int inputport = frame->arrivalGate()->index();
        buffer[inputport].queue.insert(frame);
        buffer[inputport].port = inputport;
        bufferUsed += length;

        // send PAUSE if above watermark
        if (pauseUnits>0 && highWatermark>0 && bufferUsed>=highWatermark && simTime()-pauseLastSent>pauseInterval)
        {
            // send PAUSE on all ports
            for (int i=0; i<numPorts; i++)
                sendPauseFrame(i, pauseUnits);
            pauseLastSent = simTime();
        }

        if (buffer[inputport].cpuBusy)
        {
            EV << "Port CPU " << inputport << " busy, incoming frame " << frame << " enqueued for later processing\n";
        }
        else
        {
            EV << "Port CPU " << inputport << " free, begin processing of incoming frame " << frame << endl;
            buffer[inputport].cpuBusy = true;
            cMessage *msg = new cMessage("endProcessing");
            msg->setContextPointer(&buffer[inputport]);
            scheduleAt(simTime() + processingTime, msg);
        }
    }
    // Drop the frame and record the number of dropped frames
    else
    {
        EV << "Buffer full, dropping frame " << frame << endl;
        delete frame;
        ++numDroppedFrames;
    }

    // Record statistics of buffer usage levels
    bufferLevel.record(bufferUsed);
}

void EnhancedMACRelayUnitPP::processFrame(cMessage *msg)
{
    // Extract frame from the appropriate buffer;
    PortBuffer *pBuff = (PortBuffer*)msg->contextPointer();
    EtherFrame *frame = (EtherFrame*)pBuff->queue.pop();
    long length = frame->byteLength();
    int inputport = pBuff->port;

    EV << "Port CPU " << inputport << " completed processing of frame " << frame << endl;

    handleAndDispatchFrame(frame, inputport);
    //printAddressTable();

    bufferUsed -= length;
    bufferLevel.record(bufferUsed);

    numProcessedFrames++;

    // Process next frame in queue if they are pending
    if (!pBuff->queue.empty())
    {
        EV << "Begin processing of next frame\n";
        scheduleAt(simTime()+processingTime, msg);
    }
    else
    {
        EV << "Port CPU idle\n";
        pBuff->cpuBusy = false;
        delete msg;
    }
}

void EnhancedMACRelayUnitPP::finish()
{
    if (par("writeScalars").boolValue())
    {
        recordScalar("processed frames", numProcessedFrames);
        recordScalar("dropped frames", numDroppedFrames);
    }
}

size_t EnhancedMACRelayUnitPP::getNumActivePorts()
{
    cModule* etherSwitch = parentModule();
    assert(0 != etherSwitch);
    cModule* cluster = etherSwitch->parentModule();
    assert(0 != cluster);
    int numCPUNodes = cluster->par("numCPUNodes");
    int numIONodes = cluster->par("numIONodes");
    return numCPUNodes + numIONodes;
}
