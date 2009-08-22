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
{
    parameters:
        bool useCollectiveCreate;
        bool useCollectiveGetAttr;
        bool useCollectiveRemove;
        double clientOverheadDelaySecs;
        double directoryCreateProcessingDelaySecs;
        double directoryReadProcessingDelaySecs;
        double directoryRemoveProcessingDelaySecs;
        double fileCloseProcessingDelaySecs;
        double fileDeleteProcessingDelaySecs;
        double fileOpenProcessingDelaySecs;
        double fileReadProcessingDelaySecs;
        double fileStatProcessingDelaySecs;
        double fileUpdateTimeProcessingDelaySecs;
        double fileWriteProcessingDelaySecs;

    gates:
        input appIn;
        input netIn;
        output appOut;
        output netOut;
}
