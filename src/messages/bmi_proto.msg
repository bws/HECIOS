//
// This file is part of Hecios
//
// Copyright (C) 2007,2008,2009 Brad Settlemyer
//
// This file is distributed WITHOUT ANY WARRANTY. See the file 'License.txt'
// for details on this and other legal matters.
//

//
// BMI wrapper messages for transporting data from the client to server
//
cplusplus {{
#include "basic_types.h"
}}

// Forward declarations
class noncobject ConnectionId;

// The abstract base class for all BMI Messages
message spfsBMIMessage
{
    fields:
        int tag;
};

// Message sent over an established BMI connection 
message spfsBMIExpectedMessage extends spfsBMIMessage
{
    fields:
        ConnectionId connectionId;
};

// Message sent that establishes a BMI connection
message spfsBMIUnexpectedMessage extends spfsBMIMessage
{
    fields:
        unsigned long handle;
};

// Message sent as part of a data flow
message spfsBMIFlowMessage extends spfsBMIExpectedMessage
{
    fields:
        int flowId;
        unsigned long flowSize;
};

// Message that sends data over a BMI connection
message spfsBMIPushDataRequest extends spfsBMIFlowMessage
{
    fields:
        int handle;
        unsigned long dataSize;
};

// Message that acknowledges data sent over a BMI connection
message spfsBMIPushDataResponse extends spfsBMIFlowMessage
{
    fields:
        unsigned long receivedSize;
};

//
// Local variables:
//  indent-tabs-mode: nil
//  c-indent-level: 4
//  c-basic-offset: 4
// End:
//
// vim: ts=4 sts=4 sw=4 expandtab foldmethod=marker
//
