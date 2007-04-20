

// Description of the interface for all FSClients
simple FSClient

    gates:

        in: appIn, netIn;
        out: appOut, netOut;
endsimple

// Walt's implementation of the FSClient interface
simple fsModule

    gates:

        in: appIn, netIn;
        out: appOut, netOut;
endsimple
