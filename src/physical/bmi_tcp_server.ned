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
simple BMITcpServer

    parameters:
        listenPort: numeric const, // port to listen on
        fixedOverheadSecs: numeric const,
        scaledOverheadSecs : numeric const;

    gates:
        in: appIn;
        out: appOut;

        in: tcpIn;
        out: tcpOut;

endsimple
