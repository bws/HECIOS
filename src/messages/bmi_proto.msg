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

enum spfsBMIMessageKinds
{
    SPFS_BMI_CLIENT_SEND = 401;
    SPFS_BMI_CLIENT_RECV = 402;
    SPFS_BMI_SERVER_SEND = 403;
    SPFS_BMI_SERVER_RECV = 404;
};

// The abstract base class for all BMI Messages
message spfsBMIMessage
{
    fields:
        unsigned long uniqueId;
};

// Message sent from a BMI client 
message spfsBMIClientSendMessage extends spfsBMIMessage
{
};

// Message sent from a BMI server 
message spfsBMIServerSendMessage extends spfsBMIMessage
{
};

//
// Local variables:
//  c-indent-level: 4
//  c-basic-offset: 4
// End:
//
// vim: ts=4 sts=4 sw=4 expandtab foldmethod=marker
//
