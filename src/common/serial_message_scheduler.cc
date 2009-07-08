//
// This file is part of Hecios
//
// Copyright (C) 2007,2008,2009 Brad Settlemyer
//
// This file is distributed WITHOUT ANY WARRANTY. See the file 'License.txt'
// for details on this and other legal matters.
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
