#ifndef DIRECT_MESSAGE_INTERFACE_H_
#define DIRECT_MESSAGE_INTERFACE_H_
//
// This file is part of Hecios
//
// Copyright (C) 2007,2008,2009 Brad Settlemyer
//
// This file is distributed WITHOUT ANY WARRANTY. See the file 'License.txt'
// for details on this and other legal matters.
//
class cMessage;

class DirectMessageInterface
{
public:
    /** Constructor */
    DirectMessageInterface() {};

    /** Destructor */
    virtual ~DirectMessageInterface() {};

    /** Receive and schedule a direct message */
    virtual void directMessage(cMessage* msg) = 0;

private:
};


#endif /* DIRECT_MESSAGE_INTERFACE_H_ */
