//
// This file is part of Hecios
//
// Copyright (C) 2007 Brad Settlemyer
//
// This file is distributed WITHOUT ANY WARRANTY. See the file 'License.txt'
// for details on this and other legal matters.
//
import inet.applications.tcpapp.TCPApp;

// Implements the TCPApp interface (gates)
simple MPITcpClient like TCPApp
{
    gates:
        input appIn;
        output appOut;

        input tcpIn;
        output tcpOut;
}

