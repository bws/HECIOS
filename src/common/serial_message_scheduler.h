#ifndef SERIAL_MESSAGE_SCHEDULER_H
#define SERIAL_MESSAGE_SCHEDULER_H
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
#include <omnetpp.h>

/**
 * Used to ensure that delayed messages are scheduled in serial (FIFO) order
 * with additional delay added to the schedule
 */
class SerialMessageScheduler
{
public:
    /** Constructor */
    SerialMessageScheduler(const cSimpleModule* const module);

    /** Destructor */
    ~SerialMessageScheduler();

    /**
     * @return the amount of delay from the current time until
     *  this message may be sent.  For use with sendDelayed().
     * @param any additional delay to add to the message schedule
     */
    simtime_t getNextMessageScheduleDelay(simtime_t additionalDelay);
    
    /**
     * @return the earliest available message scheduling time.
     *   For use with scheduleAt().
     * @param any additional delay to add to the message schedule
     */
    simtime_t getNextMessageScheduleTime(simtime_t additionalDelay);
    
private:
    /** Copy constructor disabled */
    SerialMessageScheduler(const SerialMessageScheduler& other);

    /** Assignment operator disabled */
    SerialMessageScheduler& operator=(const SerialMessageScheduler& other);
    
    /** The module to perform scheduling for */
    const cSimpleModule* const module_;

    /** The scheduled send time of the next message */
    simtime_t nextMessageScheduleTime_;
};

#endif

/*
 * Local variables:
 *  indent-tabs-mode: nil
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 *
 * vim: ts=8 sts=4 sw=4 expandtab
 */
