#ifndef SERIAL_MESSAGE_SCHEDULER_H
#define SERIAL_MESSAGE_SCHEDULER_H
//
// This file is part of Hecios
//
// Copyright (C) 2007,2008,2009 Brad Settlemyer
//
// This file is distributed WITHOUT ANY WARRANTY. See the file 'License.txt'
// for details on this and other legal matters.
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
