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

{
    parameters:
        double handlesPerServer;
        double metaDataSizeInBytes;
        double changeDirEntProcessingDelaySecs;
        double createDirEntProcessingDelaySecs;
        double createDFileProcessingDelaySecs;
        double createDirectoryProcessingDelaySecs;
        double createMetadataProcessingDelaySecs;
        double getAttrProcessingDelaySecs;
        double lookupPathProcessingDelaySecs;
        double readDirProcessingDelaySecs;
        double removeDirEntProcessingDelaySecs;
        double removeMetaProcessingDelaySecs;
        double removeObjectProcessingDelaySecs;
        double setAttrProcessingDelaySecs;
        double serverOverheadDelaySecs;
        bool collectDiskData;

}
