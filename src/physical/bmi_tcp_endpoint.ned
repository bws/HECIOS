//
// This file is part of Hecios
//
// Copyright (C) 2007,2008,2009 Brad Settlemyer
//
// This file is distributed WITHOUT ANY WARRANTY. See the file 'License.txt'
// for details on this and other legal matters.
//

//
// Implements the BMIEndpoint and TCPApp interfaces (gates)
//
simple BMITcpEndpoint

{
    parameters:
        // Port endpoint should bind to listen on (set to 0 to disable listen)
        double listenPort;

        // Port to contact other servers on
        double connectPort;

    gates:
        input appIn;
        output appOut;

        input tcpIn;
        output tcpOut;

}
