//
// This file is part of Hecios
//
// Copyright (C) 2007,2008,2009 Brad Settlemyer
//
// This file is distributed WITHOUT ANY WARRANTY. See the file 'License.txt'
// for details on this and other legal matters.
//

module StorageLayer

{
    parameters:
        string ioLibraryType;

    gates:
        input in;
        input fromOS;
        output out;
        output toOS;

    submodules:

        ioLibrary: <ioLibraryType> like IOLibrary {
            parameters:
                @display("p=80,80;i=block/layer,white");

        }
    connections:

        in --> ioLibrary.in;
        out <-- ioLibrary.out;

        ioLibrary.request --> toOS;
        ioLibrary.response <-- fromOS;
}

