//
// This file is part of Hecios
//
// Copyright (C) 2007,2008,2009 Brad Settlemyer
//
// This file is distributed WITHOUT ANY WARRANTY. See the file 'License.txt'
// for details on this and other legal matters.
//

module StorageLayer

    parameters:
        ioLibraryType : string;

    gates:
        in: in, fromOS;
        out: out, toOS;

    submodules:

        ioLibrary: ioLibraryType like IOLibrary;
            display: "p=80,80;i=block/layer,white";

    connections:

    in --> ioLibrary.in;
    out <-- ioLibrary.out;

    ioLibrary.request --> toOS;
    ioLibrary.response <-- fromOS;
endmodule

//
// Local variables:
//  indent-tabs-mode: nil
//  c-indent-level: 4
//  c-basic-offset: 4
// End:
//
// vim: ts=4 sts=4 sw=4 expandtab
//
