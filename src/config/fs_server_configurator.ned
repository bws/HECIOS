//
// This file is part of Hecios
//
// Copyright (C) 2007,2008,2009 Brad Settlemyer
//
// This file is distributed WITHOUT ANY WARRANTY. See the file 'License.txt'
// for details on this and other legal matters.
//

//
// File system data server configuration
//
simple FSServerConfigurator

    parameters:
        handlesPerServer: const,
        metaDataSizeInBytes: const,
        changeDirEntProcessingDelaySecs: numeric const,
        createDirEntProcessingDelaySecs: numeric const,
        createDFileProcessingDelaySecs: numeric const,
        createDirectoryProcessingDelaySecs: numeric const,
        createMetadataProcessingDelaySecs: numeric const,
        getAttrProcessingDelaySecs: numeric const,
        lookupPathProcessingDelaySecs: numeric const,
        readDirProcessingDelaySecs: numeric const,
        removeDirEntProcessingDelaySecs: numeric const,
        removeMetaProcessingDelaySecs: numeric const,
        removeObjectProcessingDelaySecs: numeric const,
        setAttrProcessingDelaySecs: numeric const,
        serverOverheadDelaySecs: numeric const,
        collectDiskData: bool;

endsimple
