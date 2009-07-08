//
// This file is part of Hecios
//
// Copyright (C) 2007,2008,2009 Brad Settlemyer
//
// This file is distributed WITHOUT ANY WARRANTY. See the file 'License.txt'
// for details on this and other legal matters.
//

// Implements the TCPApp and BMIEndpoint interface (gates)
simple BMITcpClient

    parameters:
        connectPort: numeric const, // port to connect to
        fixedOverheadSecs: numeric const,
        scaledOverheadSecs : numeric const;

    gates:
        in: appIn;
        out: appOut;

        in: tcpIn;
        out: tcpOut;

endsimple

//
// Local variables:
//  indent-tabs-mode: nil
//  c-indent-level: 4
//  c-basic-offset: 4
// End:
//
// vim: ts=4 sts=4 sw=4 expandtab foldmethod=marker
//
