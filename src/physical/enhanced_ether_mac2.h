#ifndef ENHANCED_ETHER_MAC2_H
#define ENHANCED_ETHER_MAC2_H
//
// Copyright (C) 2009 Brad Settlemyer
// Copyright (C) 2006 Levente Meszaros
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
#include <omnetpp.h>
#include "INETDefs.h"
#include "EtherFrame_m.h"
#include "enhanced_ether_mac_base.h"

/**
 * EtherMAC2 implementation.
 */
class INET_API EnhancedEtherMAC2 : public EnhancedEtherMACBase
{
  public:
    EnhancedEtherMAC2();

  protected:
    virtual void initialize();
    virtual void initializeTxrate();
    virtual void handleMessage(cMessage *msg);

    // event handlers
    virtual void startFrameTransmission();
    virtual void processFrameFromUpperLayer(EtherFrame *frame);
    virtual void processMsgFromNetwork(cMessage *msg);
    virtual void handleEndIFGPeriod();
    virtual void handleEndTxPeriod();
};

#endif
