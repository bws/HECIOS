
//
// File system data server
//
simple FSServer

    parameters:
        //name: string const;

    gates:
        in: netIn;
        out: netOut;

        in: storageIn;
        out: storageOut;

        out: schedulerOut;

endsimple