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

{
    parameters:
        double bufferSizeInBytes;
        double numBuffers;

    gates:
        input netIn;
        input directIn;
        output netOut;

        input storageIn;
        output storageOut;

        input pfsIn;
        output pfsOut;
}

