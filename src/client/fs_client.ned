//
// This file is part of Hecios
//
// Copyright (C) 2007,2008,2009 Brad Settlemyer
//
// This file is distributed WITHOUT ANY WARRANTY. See the file 'License.txt'
// for details on this and other legal matters.
//

// Description of the interface for all FSClients
simple FSClient
    parameters:
        useCollectiveCreate: bool,
        useCollectiveGetAttr: bool,
        useCollectiveRemove: bool,
        clientOverheadDelaySecs: numeric const,
        directoryCreateProcessingDelaySecs: numeric const,
        directoryReadProcessingDelaySecs: numeric const,
        directoryRemoveProcessingDelaySecs: numeric const,
        fileCloseProcessingDelaySecs: numeric const,
        fileDeleteProcessingDelaySecs: numeric const,
        fileOpenProcessingDelaySecs: numeric const,
        fileReadProcessingDelaySecs: numeric const,
        fileStatProcessingDelaySecs: numeric const,
        fileUpdateTimeProcessingDelaySecs: numeric const,
        fileWriteProcessingDelaySecs: numeric const;

    gates:
        in: appIn, netIn;
        out: appOut, netOut;
endsimple
