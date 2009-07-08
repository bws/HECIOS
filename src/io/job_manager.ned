//
// This file is part of Hecios
//
// Copyright (C) 2007,2008,2009 Brad Settlemyer
//
// This file is distributed WITHOUT ANY WARRANTY. See the file 'License.txt'
// for details on this and other legal matters.
//

//
// Server job manager
//
simple JobManager

    parameters:
        bufferSizeInBytes : const,
        numBuffers : const;

    gates:
        in: netIn, directIn;
        out: netOut;

        in: storageIn;
        out: storageOut;

        in: pfsIn;
        out: pfsOut;
endsimple
