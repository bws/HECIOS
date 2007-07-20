
// Implements the TCPApp interface (gates)
simple MPITcpServer

    parameters:
        listenPort: numeric const; // port to listen on

    gates:
        in: appIn;
        out: appOut;

        in: tcpIn;
        out: tcpOut;

endsimple
