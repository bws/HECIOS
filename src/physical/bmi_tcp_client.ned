
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
