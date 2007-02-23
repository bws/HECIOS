
// Implements the TCPApp interface (gates)
simple BMITcpServer

    parameters:
        listenPort: numeric const; // port to listen on

    gates:
        in: bmiIn;
        out: bmiOut;

        in: tcpIn;
        out: tcpOut;

endsimple
