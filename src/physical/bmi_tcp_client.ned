//
// This file is part of Hecios
//
// Copyright (C) 2007,2008,2009 Brad Settlemyer
//
// This file is distributed WITHOUT ANY WARRANTY. See the file 'License.txt'
// for details on this and other legal matters.
//
import inet.applications.tcpapp.TCPApp;

// Implements the TCPApp and BMIEndpoint interface (gates)
simple BMITcpClient like TCPApp

{
    parameters:
    	@class(BMITcpClient);
        double connectPort; // port to connect to
        double fixedOverheadSecs;
        double scaledOverheadSecs;

    gates:
        input appIn;
        output appOut;

        input tcpIn;
        output tcpOut;

}

