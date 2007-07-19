
// Implements the TCPApp interface (gates)
simple MPITcpClient

    parameters:
        connectPort: numeric const; // port to connect to

    gates:
        in: mpiClientIn;
        out: mpiClientOut;

        in: tcpIn;
        out: tcpOut;

endsimple
