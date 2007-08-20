//
// This file is part of Hecios
//
// Copyright (C) 2006 Joel Sherrill <joel@oarcorp.com>
// Copyright (C) 2007 Brad Settlemyer
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

#include "disk_scheduler.h"
#include <cassert>
#include "os_proto_m.h"
using namespace std;

DiskScheduler::DiskScheduler()
{
}

DiskScheduler::~DiskScheduler()
{
}

void DiskScheduler::initialize()
{
    inGateId_ = gate("in")->id();
    outGateId_ = gate("out")->id();
    requestGateId_ = gate("request")->id();
    
    // Initialize derived schedulers
    initializeScheduler();
}

void DiskScheduler::finish()
{
}

void DiskScheduler::handleMessage( cMessage *msg )
{
    if (msg->arrivalGateId() == inGateId_)
    {
        // If the scheduler is empty, send this entry to disk
        // otherwise, add it to the scheduler
        if (isEmpty())
        {
            send(msg, requestGateId_);
        }
        else
        {
            // Create a scheduler entry for this request
            SchedulerEntry* thisEntry = 0;
            if (spfsOSReadDeviceRequest* read =
                dynamic_cast<spfsOSReadDeviceRequest*>(msg))
            {
                thisEntry = new SchedulerEntry();
                thisEntry->lba = read->getAddress();
                thisEntry->request = msg;
                thisEntry->isReadRequest = true;
            }
            else if (spfsOSWriteDeviceRequest* write =
                     dynamic_cast<spfsOSWriteDeviceRequest*>(write))
            {
                thisEntry = new SchedulerEntry();
                thisEntry->lba = write->getAddress();
                thisEntry->request = msg;            
                thisEntry->isReadRequest = false;
            }

            // Add this entry to the scheduler
            assert(0 != thisEntry);
            addEntry(thisEntry);
        }
    }
    else
    {
        // See if the completed request satisfies any other requests
        vector<SchedulerEntry*> completedReqs;
        if (spfsOSReadDeviceResponse* read =
            dynamic_cast<spfsOSReadDeviceResponse*>(msg))
        {
            spfsOSReadDeviceRequest* readReq =
                static_cast<spfsOSReadDeviceRequest*>(read->contextPointer());
            LogicalBlockAddress readBlock = readReq->getAddress();
            completedReqs = popRequestsCompletedByRead(readBlock);
        }
        else if (spfsOSWriteDeviceResponse* write =
                 dynamic_cast<spfsOSWriteDeviceResponse*>(msg))
        {
            spfsOSWriteDeviceRequest* writeReq =
               static_cast<spfsOSWriteDeviceRequest*>(write->contextPointer());
            LogicalBlockAddress writtenBlock = writeReq->getAddress();
            completedReqs = popRequestsCompletedByWrite(writtenBlock);
            
        }

        // Construct the responses for the completed requests
        for (size_t i = 0; i < completedReqs.size(); i++)
        {
            cMessage* resp = 0;
            if (completedReqs[i]->isReadRequest)
            {
                resp = new spfsOSReadDeviceResponse();
            }
            else
            {
                resp = new spfsOSWriteDeviceResponse();
            }
            resp->setContextPointer(completedReqs[i]->request);
            send(resp, outGateId_);
            delete completedReqs[i];
        }

        // Forward the device's response
        send(msg, outGateId_);

        // Schedule the next entry
        if (!isEmpty())
        {
            SchedulerEntry* next = popNextEntry();
            send(next->request, outGateId_);
            delete next;
        }
    }
}

//------------------------------------------------
//
//  FCFSScheduler
//
//  Simple FIFO Disk Scheduler
Define_Module_Like(FCFSDiskScheduler, DiskScheduler)

FCFSDiskScheduler::FCFSDiskScheduler()
{
}

void FCFSDiskScheduler::addEntry(SchedulerEntry* entry)
{
    fcfsQueue.insert(entry);
}

SchedulerEntry* FCFSDiskScheduler::popNextEntry()
{
    return static_cast<SchedulerEntry*>(fcfsQueue.pop());
}

vector<SchedulerEntry*> FCFSDiskScheduler::popRequestsCompletedByRead(
    LogicalBlockAddress lba)
{
    vector<SchedulerEntry*> completed;
    return completed;
}

vector<SchedulerEntry*> FCFSDiskScheduler::popRequestsCompletedByWrite(
    LogicalBlockAddress lba)
{
    vector<SchedulerEntry*> completed;
    return completed;
}

/*
//------------------------------------------------
//
//  SSTFScheduler
//
//  Shortest Seek Time First Disk Scheduler
Define_Module_Like(SSTFDiskScheduler, DiskScheduler)

SSTFDiskScheduler::SSTFDiskScheduler()
{
  current_block = 0;
}

cMessage *SSTFDiskScheduler::getNextToDo( void )
{
  cMessage *msg;
  cMessage *best_msg = NULL;
  long shortest_distance = LONG_MAX;
  long d;

  if ( queue.empty() )
    return NULL;

  // Now grab as many messages as we can at the highest priority
  // that has allocated units left to do.

  for ( cQueueIterator iter(queue, 1); !iter.end() ; iter++ ) {
    msg = (cMessage *) iter();
    d = abs(current_block - msg->par("block").longValue());

    if ( d < shortest_distance ) {
       best_msg = msg;
       shortest_distance = d;
    }

  }

  //  This should never happen, but ...

  if ( !best_msg )
    return NULL;

  best_msg = (cMessage *) queue.remove( best_msg );
  return best_msg;
}

//------------------------------------------------
//
//  CScanScheduler
//
//  CSCAN Disk Scheduler -- Each sweep of the disk is in the same
//  direction.  In this implementation, the block with the lowest
//  number is serviced first.

Define_Module_Like( CScanDiskScheduler, DiskScheduler )

CScanDiskScheduler::CScanDiskScheduler()
{
}

void CScanDiskScheduler::setup()
{
    //BWS queue.setup( CompareBlocksNumber, true );
}

//------------------------------------------------
//
//  NStepCScanScheduler
//
//  NStep CSCAN Disk Scheduler -- Each sweep of the disk is in the same
//  direction.  In this implementation, the block with the lowest
//  number is serviced first.  By adding NStep, the possibility of
//  indefinite postponement is reduced.
//
//  Implementation Notes:
//
//  This algorithm uses both queues.

Define_Module_Like( NStepCScanDiskScheduler, DiskScheduler )

NStepCScanDiskScheduler::NStepCScanDiskScheduler()
{
}

void NStepCScanDiskScheduler::setup()
{
    //BWS queue.setup( CompareBlocksNumber, false );
    nstep_queue.setup( NULL, true );
}

void NStepCScanDiskScheduler::insertNewRequest( cMessage *msg )
{
  //  If servicing the primary queue, then put the new request
  //  on the secondary queue.

  ev << className() << ": Inserting " << msg->name() << " on ";
  nstep_queue.insert( msg );
  ev << "secondary queue" << endl;
}

cMessage *NStepCScanDiskScheduler::getNextToDo( void )
{
  cMessage *msg;

  if ( queue.empty() ) {
    if ( nstep_queue.empty() )
      return NULL;

    // ev << className() << ": Switching to primary queue" << endl;

    while ( !nstep_queue.empty() ) {
      msg = (cMessage *) nstep_queue.pop();
      //BWS ev << className() << ": Moving " << msg->name() << endl;
      queue.insert( msg );
    }
  }

  return (cMessage *)queue.pop();
}


//------------------------------------------------
//
//  ScanScheduler
//
//  SCAN Disk Scheduler -- each sweep of the disk is in a different
//  direction.  On one pass, the block with the lowest number is
//  serviced first.  On the next pass, the block with the highest
//  number is serviced.

Define_Module_Like( ScanDiskScheduler, DiskScheduler )

ScanDiskScheduler::ScanDiskScheduler()
{
}

void ScanDiskScheduler::setup()
{
  going_up = false;
}

void ScanDiskScheduler::insertNewRequest( cMessage *msg )
{
  if ( queue.empty() ) {

    ev << className() << ": Switching directions from " ;
    if ( going_up ) ev << "UP";
    else            ev << "DOWN";

    going_up = (going_up) ? false : true;
    //BWS queue.setup( CompareBlocksNumber, !going_up );

    ev << " to ";
    if ( going_up ) ev << "UP";
    else            ev << "DOWN";
    ev << endl;
  }

  ev << className() << ": Inserting " << msg->name() << endl;
  queue.insert( msg );
}

// ScanScheduler::getNextToDo
//
//  Behavior is the same as AbstractScheduler


//------------------------------------------------
//
//  NStepScanScheduler
//
//  N-Step SCAN Disk Scheduler -- each sweep of the disk is in a different
//  direction.  On one pass, the block with the lowest number is
//  serviced first.  On the next pass, the block with the highest
//  number is serviced.
//
//  Implementation Notes:
//
//  This algorithm uses both queues.

Define_Module_Like( NStepScanDiskScheduler, DiskScheduler )

NStepScanDiskScheduler::NStepScanDiskScheduler()
{
}

void NStepScanDiskScheduler::setup()
{
  going_up = false;
  nstep_queue.setup( NULL, true );
}

void NStepScanDiskScheduler::insertNewRequest( cMessage *msg )
{
  //  If servicing the primary queue, then put the new request
  //  on the secondary queue.

  ev << className() << ": Inserting " << msg->name() << " on ";
  nstep_queue.insert( msg );
  ev << "secondary queue" << endl;
}

cMessage *NStepScanDiskScheduler::getNextToDo( void )
{
  cMessage *msg;

  if ( queue.empty() ) {
    if ( nstep_queue.empty() )
      return NULL;

    // ev << className() << ": Switching to primary queue" << endl;

    ev << className() << ": Switching directions from " ;
    if ( going_up ) ev << "UP";
    else            ev << "DOWN";

    going_up = (going_up) ? false : true;
    //BWS queue.setup( CompareBlocksNumber, !going_up );

    ev << " to ";
    if ( going_up ) ev << "UP";
    else            ev << "DOWN";
    ev << endl;

    while ( !nstep_queue.empty() ) {
      msg = (cMessage *) nstep_queue.pop();
      ev << className() << ": Moving " << msg->name() << endl;
      queue.insert( msg );
    }
  }

  return (cMessage *)queue.pop();
}
*/

/*
 * Local variables:
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 *
 * vim: ts=8 sts=4 sw=4 expandtab
 */
