
// Implements the TCPApp interface (gates)
simple BMITcpClient

    parameters:
        connectPort: numeric const; // port to connect to

    gates:
        in: bmiIn;
        out: bmiOut;

        in: tcpIn;
        out: tcpOut;

endsimple

//
// Local variables:
//  c-indent-level: 4
//  c-basic-offset: 4
// End:
//
// vim: ts=4 sts=4 sw=4 expandtab foldmethod=marker
//
