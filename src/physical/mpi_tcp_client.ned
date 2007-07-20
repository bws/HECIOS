
// Implements the TCPApp interface (gates)
simple MPITcpClient

    parameters:
        connectPort: numeric const; // port to connect to

    gates:
        in: appIn;
        out: appOut;

        in: tcpIn;
        out: tcpOut;

endsimple
