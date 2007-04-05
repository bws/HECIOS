/**
 * @file block_compare.cc
 *
 * Note: These classes are adapted from the FSS simulator project and are
 * licensed only under the GPL.  The FSS project is available at:
 * http://www.omnetpp.org/filemgmt/singlefile.php?lid=104 and
 * http://www.omnetpp.org/doc/FSS-doc/neddoc/index.html
 */

#include <omnetpp.h>
#include "block_compare.h"

extern "C" {

  //  CompareBlocksTimeStamp:: Compares the timestamps of two block messages
  int CompareBlocksTimeStamp(cObject *left, cObject *right)
  {
    cMessage *leftMsg = (cMessage *)left;
    cMessage *rightMsg = (cMessage *)right;
    double d = leftMsg->timestamp() - rightMsg->timestamp();

    // if left <  right   return a negative value
    // if left == right   return 0
    // if left >  right   return a postive value

    if ( d < 0.0 )
      return -1;

    if ( d > 0.0 )
      return 1;

    return 0;
  }

  //  CompareBlocksNumber:: Compares the block numbers of two block messages
  int CompareBlocksNumber(cObject *left, cObject *right)
  {
    cMessage *leftMsg = (cMessage *)left;
    cMessage *rightMsg = (cMessage *)right;
    cPar& leftPar = leftMsg->par("block");
    cPar& rightPar = rightMsg->par("block");

    int d = leftPar.longValue() - rightPar.longValue();

    // if left <  right   return a negative value
    // if left == right   return 0
    // if left >  right   return a postive value

    if ( d < 0 )
      return -1;

    if ( d > 0 )
      return 1;

    return 0;
  }

  //  CompareBlocksPriority:: Compares the priority of two block messages.
  //     A numerically lower number indicates greater importance.
  int CompareBlocksPriority(cObject *left, cObject *right)
  {
    cMessage *leftMsg = (cMessage *)left;
    cMessage *rightMsg = (cMessage *)right;
    cPar& leftPar = leftMsg->par("priority");
    cPar& rightPar = rightMsg->par("priority");

    int d = leftPar.longValue() - rightPar.longValue();

    // if left <  right   return a negative value
    // if left == right   return 0
    // if left >  right   return a postive value

    if ( d < 0 )
      return -1;

    if ( d > 0 )
      return 1;

    return 0;
  }

  //  CompareBlocksPriorityAscending::
  //     Compares the priority of two block messages.   If the priority
  //     of the two messages are equal, then the block numbers are
  //     used as a secondary comparison point.
  int CompareBlocksPriorityAscending(cObject *left, cObject *right)
  {
    int result;

    result = CompareBlocksPriority( left, right );
    if ( result != 0 )
      return result;

    return CompareBlocksNumber( left, right );
  }

  //  CompareBlocksPriorityDescending::
  //     Compares the priority of two block messages.   If the priority
  //     of the two messages are equal, then the block numbers are
  //     used as a secondary comparison point.
  int CompareBlocksPriorityDescending(cObject *left, cObject *right)
  {
    int result;

    result = CompareBlocksPriority( left, right );
    if ( result != 0 )
      return result;

    return CompareBlocksNumber( left, right ) * -1;
  }

  //  CompareBlocksLRUPriority::
  //     Compares the priority of two block messages.   If the priority
  //     of the two messages are equal, then the timestamps are
  //     used as a secondary comparison point.
  int CompareBlocksLRUPriority(cObject *left, cObject *right)
  {
    int result;

    result = CompareBlocksPriority( left, right );
    if ( result != 0 )
      return result;

    return CompareBlocksTimeStamp( left, right );
  }


}; /* end of extern "C" */
