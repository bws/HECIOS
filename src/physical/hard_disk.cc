/**
 * @file hard_disk.cc
 *
 * Note: These classes are adapted from the FSS simulator project and are
 * licensed only under the GPL.  The FSS project is available at:
 * http://www.omnetpp.org/filemgmt/singlefile.php?lid=104 and
 * http://www.omnetpp.org/doc/FSS-doc/neddoc/index.html
 */

#include <string.h>
#include "hard_disk.h"


AbstractDisk::
    AbstractDisk( const char *namestr, cModule *parent, size_t stack):
cSimpleModule( namestr, parent, stack),
queue("queue")
{
  diskAccess = new cOutVector( "disk-accesses", 2 );
  AccessStdDev = new cStdDev( "disk-access-time-stddev" );
  SeekStdDev = new cStdDev( "disk-seek-distance-stddev" );
  cDiskDelayMessage = new cMessage( "disk-operation-delay" );
}

AbstractDisk::~AbstractDisk()
{
  delete diskAccess;
  delete AccessStdDev;
  delete SeekStdDev;
}

void AbstractDisk::initialize()
{
  current_block = 0;
  total_distance = 0;
  start_time = simTime();
}

void AbstractDisk::finish()
{
  ev << className() << ": End of Simulation" << endl;
  // ev << className() << ": Access Time Statistics" << endl;
  // ev << className() << ": Quantity: "  << AccessStdDev->samples() << endl;
  // ev << className() << ": Minimum:  "  << AccessStdDev->min() << endl;
  // ev << className() << ": Maximum:  "  << AccessStdDev->max() << endl;
  // ev << className() << ": Mean:     "  << AccessStdDev->mean() << endl;
  // ev << className() << ": Variance: "  << AccessStdDev->variance() << endl;
  // ev << className() << ": Standard Deviation: " <<
  //       AccessStdDev->stddev() << endl;

  // ev << className() << ": Seek Distance Statistics" << endl;
  // ev << className() << ": Total:    " << total_distance << endl;
  // ev << className() << ": Quantity: " << SeekStdDev->samples() << endl;
  // ev << className() << ": Minimum:  " << SeekStdDev->min() << endl;
  // ev << className() << ": Maximum:  " << SeekStdDev->max() << endl;
  // ev << className() << ": Mean:     " << SeekStdDev->mean() << endl;
  // ev << className() << ": Variance: " << SeekStdDev->variance() << endl;
  // ev << className() << ": Standard Deviation: " <<
  //       SeekStdDev->stddev() << endl;

  AccessStdDev->recordScalar("Disk Access Time");
  SeekStdDev->recordScalar("Disk Seek Distance");
  recordScalar( "Disk Total Seek Distance", total_distance );
}

void AbstractDisk::handleMessage(cMessage *msg)
{
  long this_block = 0;
  double completeRequestAt;

  if ( !strcmp( msg->name(), "disk-operation-delay" ) ) {
    ev << className() << ": Completed service of "
       << cPendingRequestMessage->name() << " at " << simTime() << endl;
    access_time = simTime() - start_time;
    send( cPendingRequestMessage, "out" );

    // record statistics
    this_block = cPendingRequestMessage->par("block").longValue();
    this_distance = abs( this_block - current_block );

    diskAccess->record( this_block, access_time );
    AccessStdDev->collect( access_time );

    SeekStdDev->collect( this_distance );
    total_distance += this_distance;

    current_block = this_block;

    // start measuring idle time
    start_time = simTime();
    return;
  }

  // If the disk was idle, let's report it
  access_time = simTime() - start_time;
  if ( access_time > 0.0 ) {
    diskAccess->record( -1, access_time );
  }

  // Service the request
  start_time = simTime();
  cPendingRequestMessage = msg;

  completeRequestAt = service( msg );

  // Schedule the completion of this disk access
  scheduleAt(completeRequestAt, cDiskDelayMessage);
}

//------------------------------------------------
//
// ACPDisk : Fake disk with given service time

Define_Module_Like( ACPDisk, APhysicalDisk )

ACPDisk::
    ACPDisk( const char *namestr, cModule *parent, size_t stack):
AbstractDisk( namestr, parent, stack)
{
}

double ACPDisk::service(cMessage *msg)
{
  ev << className() << ": Starting service of " << msg->name() <<
     " at " << simTime() << endl;
  return simTime() + (double)par("service_time");
}

//
//  Hewlett Packard HP-97560 Disk Drive Model
//

#include <math.h>  /* for sqrt */

Define_Module_Like( HP97560Disk, APhysicalDisk )

HP97560Disk::
    HP97560Disk( const char *namestr, cModule *parent, size_t stack):
AbstractDisk( namestr, parent, stack)
{
  last_cylinder = 0;
  last_time = 0;
}

#define HP97560_FIXED_CONTROLLER_READ_OVEREHEAD  (double)0.0022
#define HP97560_FIXED_CONTROLLER_WRITE_OVEREHEAD (double)0.0022
#define HP97560_TRACK_SWITCH_TIME                (double)0.0016

#define HP97560_CYLINDERS                 1962
#define HP97560_TRACKS_PER_CYLINDER       19
#define HP97560_DATA_SECTORS_PER_TRACK    72
#define HP97560_DATA_SECTORS_PER_CYLINDER (HP97560_DATA_SECTORS_PER_TRACK * HP97560_TRACKS_PER_CYLINDER)
#define HP97560_TOTAL_SECTORS             (HP97560_DATA_SECTORS_PER_CYLINDER * HP97560_CYLINDERS)
#define HP97560_RPMS                      4002
#define HP97560_TIME_PER_REVOLUTION       ((double)(1.0 / HP97560_RPMS))
#define HP97560_TIME_PER_SECTOR           (double)((double)HP97560_TIME_PER_REVOLUTION / HP97560_DATA_SECTORS_PER_CYLINDER)


double HP97560Disk::service(cMessage *msg)
{
    long dest_block;
    long dest_cylinder;
    long cylinders_to_move;
    long current_sector;
    long dest_sector;
    long sectors_to_move;
    long current_track;
    long dest_track;
    double delay;
    double totalDelay = 0.0;

    dest_block = ((cPar&)msg->par("block")).longValue();
    dest_cylinder = dest_block / HP97560_DATA_SECTORS_PER_CYLINDER;
    dest_sector = dest_block % HP97560_DATA_SECTORS_PER_CYLINDER;

    ev << className() << ": Starting service of " << msg->name() <<
       " at " << simTime() <<
       " (" << dest_cylinder << ", " << dest_sector << ")" << endl;

    // account for fixed controller overhead

    if ( msg->par("is_read").boolValue() )
      delay = HP97560_FIXED_CONTROLLER_READ_OVEREHEAD;
    else
      delay = HP97560_FIXED_CONTROLLER_WRITE_OVEREHEAD;

    totalDelay += delay;

    // get to the right cylinder

    cylinders_to_move = abs(dest_cylinder - last_cylinder);
    if ( cylinders_to_move ) {
      if ( cylinders_to_move <= 383 )
        delay = (3.24 + 0.400 * sqrt((double)cylinders_to_move)) / 1000.0;
      else
        delay = (8.00 + 0.008 * cylinders_to_move) / 1000.0;

      // delay
      totalDelay += delay;
      // ev << className() << ": Move " << cylinders_to_move << " cylinders (" <<
      //     delay << ")" << endl;
    } else {
      // ev << className() << ": Move 0 cylinders" << endl;
      ;
    }

    last_cylinder = dest_cylinder;

    // now account for the rotational delay

    current_sector = (long) (fmod(simTime(),HP97560_TIME_PER_REVOLUTION) /
                  HP97560_TIME_PER_SECTOR);
    if ( current_sector != dest_sector ) {
      if ( current_sector < dest_sector ) {
        sectors_to_move = dest_sector - current_sector;
      } if ( current_sector > dest_sector ) {
        // Must wrap around to sector 0
        sectors_to_move  = (HP97560_DATA_SECTORS_PER_CYLINDER - current_sector);
        sectors_to_move += dest_sector;
      }
      delay  = sectors_to_move * HP97560_TIME_PER_SECTOR;
      // ev << className() << ": Move " << sectors_to_move << " sectors (" <<
      //    delay << ")" << endl;

      totalDelay += delay;
    } else {
      sectors_to_move = 0;
      // ev << className() << ": Move 0 cylinders" << endl;
    }

    // If we moved between tracks, then account for that

    if ( sectors_to_move > 0 ) {
      current_track = current_sector / HP97560_DATA_SECTORS_PER_TRACK;
      dest_track    = dest_sector / HP97560_DATA_SECTORS_PER_TRACK;
      if ( current_track != dest_track ) {
        delay  = abs(dest_track - current_track) * HP97560_TRACK_SWITCH_TIME;
        // ev << className() << ": Track switch delay of "  << delay << endl;
        totalDelay += delay;
      }

    }

    // Now we can transfer the data off the media

  totalDelay += HP97560_TIME_PER_SECTOR;

  return totalDelay;
}

