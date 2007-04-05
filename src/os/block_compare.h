/**
 * @file compare.h
 * @brief Comparison Methods Used For Indirect Calls
 */

/*
 *  $Id: block_compare.h,v 1.1 2007/04/05 22:57:17 bradles Exp $
 */


#ifndef __COMPARE_h
#define __COMPARE_h

#include <omnetpp.h>

extern "C" {

  //!  CompareBlocksTimeStamp:: Compares the timestamps of two block messages
  int CompareBlocksTimeStamp(cObject *left, cObject *right);

  //!  CompareBlocksNumber:: Compares the block numbers of two block messages
  int CompareBlocksNumber(cObject *left, cObject *right);

  //!  CompareBlocksPriority:: Compares the priority of two block messages.
  //!     A numerically lower number indicates greater importance.
  int CompareBlocksPriority(cObject *left, cObject *right);

  //!  CompareBlocksPriorityAscending::
  //!     Compares the priority of two block messages.   If the priority
  //!     of the two messages are equal, then the block numbers are
  //!     used as a secondary comparison point.
  int CompareBlocksPriorityAscending(cObject *left, cObject *right);

  //!  CompareBlocksPriorityDescending::
  //!     Compares the priority of two block messages.   If the priority
  //!     of the two messages are equal, then the block numbers are
  //!     used as a secondary comparison point.
  int CompareBlocksPriorityDescending(cObject *left, cObject *right);

  //!  CompareBlocksLRUPriority::
  //!     Compares the priority of two block messages.   If the priority
  //!     of the two messages are equal, then the timestamps are
  //!     used as a secondary comparison point.
  int CompareBlocksLRUPriority(cObject *left, cObject *right);

}; /* end of extern "C" */

#endif
/* end of include file */
