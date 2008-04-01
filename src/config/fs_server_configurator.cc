//
// This file is part of Hecios
//
// Copyright (C) 2007 Brad Settlemyer
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
//
#include <cassert>
#include <cstring>
#include <iostream>
#include "InterfaceTableAccess.h"
#include "IPv4InterfaceData.h"
#include "IPvXAddress.h"
#include "file_builder.h"
#include "fs_server.h"
#include "pvfs_proto_m.h"
#include "pfs_types.h"
#include "pfs_utils.h"
#include <omnetpp.h>
using namespace std;

/**
 * Configuration data for the file system servers
 */
class FSServerConfigurator : public cSimpleModule
{
public:
    /** Constructor */
    FSServerConfigurator() : cSimpleModule() {};
    
protected:

    /** Must have more stages than it takes to assign IPs */
    virtual int numInitStages() const {return 4;};
    
    /** Implementation of initialize */
    virtual void initialize(int stage);

    /** Implementation of finish */
    virtual void finish();

    /** Implementation of handleMessage */
    virtual void handleMessage(cMessage* msg);

private:

    IPvXAddress* getServerIP(cModule* ioNode);

    size_t handlesPerServer_;
};

// OMNet Registriation Method
Define_Module(FSServerConfigurator);

//
// Stage 0 - set the file system options into the layout and file building
//           frameworks
// Stage 3 - assign handle ranges and register the IP addresses for the
//           file system servers (IP addresses should be set at this time)
void FSServerConfigurator::initialize(int stage)
{
    if (0 == stage)
    {
        // Get the metadata size, which is currently ignored for good reason
        size_t metaDataSize = par("metaDataSizeInBytes");
        FSServer::setDefaultAttrSize(metaDataSize);
        FileBuilder::instance().setDefaultMetaDataSize(metaDataSize);

        // Get the server processing delays for each message
        double changeDirEntDelay = par("changeDirEntProcessingDelaySecs");
        FSServer::setChangeDirEntProcessingDelay(changeDirEntDelay);
        
        double createDFileDelay = par("createDFileProcessingDelaySecs");
        FSServer::setCreateDFileProcessingDelay(createDFileDelay);
        
        double createDirDelay = par("createDirectoryProcessingDelaySecs");
        FSServer::setCreateDirectoryProcessingDelay(createDirDelay);
        
        double createMetaDelay = par("createMetadataProcessingDelaySecs");
        FSServer::setCreateMetadataProcessingDelay(createMetaDelay);
        
        double createDirEntDelay = par("createDirEntProcessingDelaySecs"); 
        FSServer::setCreateDirEntProcessingDelay(createDirEntDelay);

        double getAttrDelay = par("getAttrProcessingDelaySecs");
        FSServer::setGetAttrProcessingDelay(getAttrDelay);

        double lookupPathDelay = par("lookupPathProcessingDelaySecs");
        FSServer::setLookupPathProcessingDelay(lookupPathDelay);

        double readDirDelay = par("readDirProcessingDelaySecs");
        FSServer::setReadDirProcessingDelay(readDirDelay);
        
        double removeDirEntDelay = par("removeDirEntProcessingDelaySecs");
        FSServer::setRemoveDirEntProcessingDelay(removeDirEntDelay);
        
        double removeMetaDelay = par("removeMetaProcessingDelaySecs");
        FSServer::setRemoveMetaProcessingDelay(removeMetaDelay);
        
        double removeObjectDelay = par("removeObjectProcessingDelaySecs");
        FSServer::setRemoveObjectProcessingDelay(removeObjectDelay);
        
        double setAttrDelay = par("setAttrProcessingDelaySecs");
        FSServer::setSetAttrProcessingDelay(setAttrDelay);

        double serverOverheadDelay = par("serverOverheadDelaySecs");
        FSServer::setServerOverheadDelay(serverOverheadDelay);

        // Get the flag controlling disk data collection
        bool collectDiskData = par("collectDiskData");
        FSServer::setCollectDiskData(collectDiskData);        
    }
    else if (3 == stage)
    {
        // Retrieve the handles per server
        handlesPerServer_ = par("handlesPerServer");
        cerr << "DIAGNOSTIC: Maximum handles per server is: "
             << handlesPerServer_ << endl;
        
        // Retrieve the interface table for this module
        cModule* cluster = parentModule();
        assert(0 != cluster);

        // Register the handles and IP for each IONode
        long numIONodes = cluster->par("numIONodes");        
        for (int i = 0; i < numIONodes; i++)
        {
            cModule* ion = cluster->submodule("ion", i);

            // Retrieve the FS server
            cModule* daemon = ion->submodule("daemon");
            assert(0 != daemon);
            FSServer* server =
                dynamic_cast<FSServer*>(daemon->submodule("pfsServer"));
            assert(0 != server);

            // Set the server's handle range
            HandleRange range;
            range.first = i * handlesPerServer_;
            range.last = ((i + 1) * handlesPerServer_) - 1;
            server->setHandleRange(range);
            
            // Register the server's handle range
            int serverNum;
            //if (0 == i)
            //{
            serverNum = FileBuilder::instance().registerFSServer(
                server->getHandleRange(), true);
            //}
            //else
            // {
            //serverNum = FileBuilder::instance().registerFSServer(
            //    server->getHandleRange(), false);
            //}
            
            // Set the server's server number (will construct ranges also)
            server->setNumber(serverNum);

            // Register the IP for the handle range
            IPvXAddress* addr = getServerIP(ion);
            PFSUtils::instance().registerServerIP(addr,
                                                  server->getHandleRange());

        }
    }
}

void FSServerConfigurator::finish()
{
    FileBuilder::clearState();
}

IPvXAddress* FSServerConfigurator::getServerIP(cModule* ioNode)
{
    assert(0 != ioNode);
    IPvXAddress* serverIP = 0;
    
    // Retrieve the INET host
    cModule* hca = ioNode->submodule("hca");
    assert(0 != hca);
    InterfaceTable* ifTable = dynamic_cast<InterfaceTable*>(
        hca->submodule("interfaceTable"));
    assert(0 != ifTable);
            
    // Find eth0's IP address
    InterfaceEntry* ie = 0;
    for (int j = 0; j < ifTable->numInterfaces(); j++)
    {
        ie = ifTable->interfaceAt(j);
        if (0 == strcmp("eth0", ie->name()))
        {
            assert(0 != ie->ipv4());
            serverIP = new IPvXAddress(ie->ipv4()->inetAddress());
            break;
        }
    }

    return serverIP;
}

/**
 * Disable message handling
 */
void FSServerConfigurator::handleMessage(cMessage* msg)
{
    cerr << "FSServerConfigurator cannot receive messages." << endl;
}

/*
 * Local variables:
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 *
 * vim: ts=8 sts=4 sw=4 expandtab
 */
