
// Implements the TCPApp interface (gates)
simple MPITcpServer

    parameters:
        listenPort: numeric const; // port to listen on

    gates:
        in: mpiServerIn;
        out: mpiServerOut;

        in: tcpIn;
        out: tcpOut;

endsimple
