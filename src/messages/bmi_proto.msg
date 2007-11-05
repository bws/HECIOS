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

//
// BMI wrapper messages for transporting data from the client to server
//

// The abstract base class for all BMI Messages
message spfsBMIMessage
{
    fields:
};

// Message sent over an established BMI connection 
message spfsBMIExpectedMessage extends spfsBMIMessage
{
    fields:
        int connectionId;
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
// Message that requests data over a connection
message spfsBMIPullDataRequest extends spfsBMIFlowMessage
{
    fields:
        int handle;
        unsigned long requestSize;
};

// Message sent that establishes a BMI connection
message spfsBMIPullDataResponse extends spfsBMIFlowMessage
{
    fields:
        unsigned long dataSize;
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
