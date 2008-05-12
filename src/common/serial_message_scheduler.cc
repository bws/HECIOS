//
// This file is part of Hecios
//
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
#include "serial_message_scheduler.h"

SerialMessageScheduler::SerialMessageScheduler(const cSimpleModule* const module)
    : module_(module),
      nextMessageScheduleTime_(0.0)
{
}

SerialMessageScheduler::~SerialMessageScheduler()
{
}

simtime_t SerialMessageScheduler::getNextMessageScheduleDelay(
    simtime_t additionalDelay)
{
    return getNextMessageScheduleTime(additionalDelay) - module_->simTime();
}

simtime_t SerialMessageScheduler::getNextMessageScheduleTime(
    simtime_t additionalDelay)
{
    // Update the schedule to the current time if necessary
    simtime_t currentTime = module_->simTime();
    if (nextMessageScheduleTime_ < currentTime)
    {
        nextMessageScheduleTime_ = currentTime;
    }

    // Add additional delay
    nextMessageScheduleTime_ += additionalDelay;
    return nextMessageScheduleTime_;
}

/*
 * Local variables:
 *  indent-tabs-mode: nil
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 *
 * vim: ts=8 sts=4 sw=4 expandtab
 */
