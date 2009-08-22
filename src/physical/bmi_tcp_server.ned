//
// This file is part of Hecios
//
// Copyright (C) 2007,2008,2009 Brad Settlemyer
//
// This file is distributed WITHOUT ANY WARRANTY. See the file 'License.txt'
// for details on this and other legal matters.
//
import inet.applications.tcpapp.TCPApp;

//
// Implements the BMIEndpoint and TCPApp interfaces (gates)
//
simple BMITcpServer like TCPApp

{
    parameters:
    	@class(BMITcpServer);
        double listenPort; // port to listen on
        double fixedOverheadSecs;
        double scaledOverheadSecs;

    gates:
        input appIn;
        output appOut;

        input tcpIn;
        output tcpOut;

}
