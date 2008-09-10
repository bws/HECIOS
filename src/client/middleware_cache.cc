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
#include "middleware_cache.h"
#include <cassert>
#include <iostream>
#include <omnetpp.h>
#include "basic_data_type.h"
#include "block_indexed_data_type.h"
#include "data_type_processor.h"
#include "file_builder.h"
#include "file_view.h"
#include "mpi_proto_m.h"
using namespace std;

MiddlewareCache::MiddlewareCache()
    : appInGateId_(-1),
      appOutGateId_(-1),
      fsInGateId_(-1),
      fsOutGateId_(-1),
      numCacheHits_(0),
      numCacheMisses_(0),
      numCacheEvicts_(0)
{
}

MiddlewareCache::~MiddlewareCache()
{
}

void MiddlewareCache::initialize()
{
    // Find gate ids
    appInGateId_ = findGate("appIn");
    appOutGateId_ = findGate("appOut");
    fsInGateId_ = findGate("fsIn");
    fsOutGateId_ = findGate("fsOut");
    
    // Initialize statistics
    numCacheHits_ = 0;
    numCacheMisses_ = 0;
    numCacheEvicts_ = 0;
}

void MiddlewareCache::finish()
{
    double totalCacheAccesses = numCacheHits_ + numCacheMisses_;
    double hitRate = numCacheHits_ / totalCacheAccesses;
    recordScalar("SPFS MWare Cache Hit Rate", hitRate);
    recordScalar("SPFS MWare Cache Evictions", numCacheEvicts_);
    recordScalar("SPFS MWare Cache Hits", numCacheHits_);
    recordScalar("SPFS MWare Cache Misses", numCacheMisses_);
}

void MiddlewareCache::handleMessage(cMessage* msg)
{
     if (msg->arrivalGateId() == appInGateId())
     {
         handleApplicationMessage(msg);
     }
     else if (msg->arrivalGateId() == fsInGateId())
     {
         handleFileSystemMessage(msg);
     }
     else
     {
         cerr << __FILE__ << ":" << __LINE__ << ":"
              <<  "message arrived through illegal gate: "
              << msg->info() << endl;
     }
}

//
// PagedCache implementation
//
//
PagedCache::PagedCache()
{
}

PagedCache::~PagedCache()
{    
}

FSOffset PagedCache::pageBeginOffset(const FilePageId& pageId) const
{
    return (pageId * pageSize_);
}

vector<FilePageId> PagedCache::determineRequestPages(const FSOffset& offset,
                                                     const FSSize& size,
                                                     const FileView& view)
{
    // Flatten view into file regions for the correct size
    vector<FileRegion> requestRegions = 
        DataTypeProcessor::locateFileRegions(offset, size, view);
    
    // Convert regions into file pages
    return regionsToPageIds(requestRegions);
}

void PagedCache::initialize()
{
    MiddlewareCache::initialize();
    pageSize_ = par("pageSize");
    pageCapacity_ = par("pageCapacity");
}

spfsMPIFileReadAtRequest* PagedCache::createPageReadRequest(
    const vector<FilePageId>& pageIds,
    spfsMPIFileReadRequest* origRequest) const
{
    // Construct a descriptor that views only the correct pages
    Filename name = origRequest->getFileDes()->getFilename();
    FileDescriptor* fd = getPageViewDescriptor(name, pageIds);
    
    // Create the read request
    spfsMPIFileReadAtRequest* readRequest =
        new spfsMPIFileReadAtRequest("PagedCache Read Request",
                                     SPFS_MPI_FILE_READ_AT_REQUEST);
    readRequest->setContextPointer(origRequest);
    readRequest->setFileDes(fd);
    readRequest->setDataType(new ByteDataType());
    readRequest->setCount(pageIds.size() * pageSize_);
    readRequest->setOffset(0);
    return readRequest;
}

spfsMPIFileWriteAtRequest* PagedCache::createPageWriteRequest(
    const vector<FilePageId>& pageIds,
    spfsMPIFileWriteRequest* origRequest) const
{
    // Construct a descriptor that views only the correct pages
    Filename name = origRequest->getFileDes()->getFilename();
    FileDescriptor* fd = getPageViewDescriptor(name, pageIds);
    
    // Create the read request
    spfsMPIFileWriteAtRequest* writeRequest =
        new spfsMPIFileWriteAtRequest("PagedCache Write Request",
                                      SPFS_MPI_FILE_WRITE_AT_REQUEST);
    writeRequest->setContextPointer(origRequest);
    writeRequest->setFileDes(fd);
    writeRequest->setDataType(new ByteDataType());
    writeRequest->setCount(pageIds.size() * pageSize_);
    writeRequest->setOffset(0);
    return writeRequest;
}

vector<FilePageId> PagedCache::regionsToPageIds(const vector<FileRegion>& fileRegions)
{
    vector<FilePageId> spanningPageIds;
    for (size_t i = 0; i < fileRegions.size(); i++)
    {
        // Determine the first and last page
        FSOffset begin = fileRegions[0].offset;
        FSOffset end = begin + fileRegions[0].extent;
        size_t firstPage = begin / pageSize_;
        size_t lastPage = end / pageSize_;
        for (size_t j = firstPage; j < lastPage; j++)
        {
            spanningPageIds.push_back(j);
        }
    }
    return spanningPageIds;
}

vector<FilePage> PagedCache::regionsToPages(const vector<FileRegion>& fileRegions)
{
    vector<FilePageId> spanningIds = regionsToPageIds(fileRegions);
    vector<FilePage> spanningPages;
    for (size_t i = 0; i < spanningIds.size(); i++)
    {
        FilePageId pageId = spanningIds[i];
        FilePage fp(pageBeginOffset(pageId), pageSize_);
        spanningPages.push_back(fp);
    }
    return spanningPages;
}

FileDescriptor* PagedCache::getPageViewDescriptor(
    const Filename& filename, const vector<size_t>& pageIds) const
{
    // Create the vector of displacements
    vector<size_t> displacements(pageIds.size());
    for (size_t i = 0; i < displacements.size(); i++)
    {
        displacements[i] = pageIds[i] * pageSize_;
    }

    // Create the block indexed data type to use as the view
    BlockIndexedDataType* pageView = new BlockIndexedDataType(pageSize_,
                                                              displacements,
                                                              ByteDataType());
    FileView cacheView(0, pageView);

    // Create a descriptor with which to apply the view
    FileDescriptor* fd = FileBuilder::instance().getDescriptor(filename);
    fd->setFileView(cacheView);    
    return fd;
}

//
// NoMiddlewareCache implementation
//
//
// OMNet Registriation Method
Define_Module(NoMiddlewareCache);

NoMiddlewareCache::NoMiddlewareCache()
{
}

// Perform simple pass through on all messages
void NoMiddlewareCache::handleApplicationMessage(cMessage* msg)
{
    send(msg, fsOutGateId());
}

void NoMiddlewareCache::handleFileSystemMessage(cMessage* msg)
{
    send(msg, appOutGateId());
}

//
// DirectPagedMiddlewareCache Implementation
//
//
// OMNet Registriation Method
Define_Module(DirectPagedMiddlewareCache);

DirectPagedMiddlewareCache::DirectPagedMiddlewareCache()
    : lruCache_(0)
{
}

void DirectPagedMiddlewareCache::handleApplicationMessage(cMessage* msg)
{
    if (SPFS_MPI_FILE_READ_AT_REQUEST == msg->kind()) 
    {
        // Determine the size of the read request
        spfsMPIFileReadAtRequest* readAt = 
            static_cast<spfsMPIFileReadAtRequest*>(msg);
        FSSize readSize = 
            readAt->getDataType()->getExtent() * readAt->getCount();

        // Convert regions into file pages
        FileDescriptor* fd = readAt->getFileDes();
        vector<FilePageId> requestPages = determineRequestPages(readAt->getOffset(),
                                                              readSize,
                                                              fd->getFileView());
        
        // Perform a lookup on the pages
        lookupData(requestPages);
    }
    else if (SPFS_MPI_FILE_READ_REQUEST == msg->kind())
    {
        // Determine the size of the read request
        spfsMPIFileReadRequest* read = static_cast<spfsMPIFileReadRequest*>(msg);
        FSSize readSize = read->getDataType()->getExtent() * read->getCount();

        // Convert regions into file pages
        FileDescriptor* fd = read->getFileDes();
        vector<FilePageId> requestPages = 
            determineRequestPages(fd->getFilePointer(),
                                  readSize,
                                  fd->getFileView());        
        
        // Perform a lookup on the pages
        lookupData(requestPages);
    }
    else
    {
        send(msg, fsOutGateId());
    }
}

void DirectPagedMiddlewareCache::handleFileSystemMessage(cMessage* msg)
{
    // Determine if the message is a write back
    if (0 == msg->contextPointer())
    {
        assert(SPFS_MPI_FILE_WRITE_AT_RESPONSE == msg->kind());
        cerr << __FILE__ << ":" << __LINE__ << ": "
             << "Cache Write-Back Complete" << endl;
    }
    else
    {
        if (SPFS_MPI_FILE_READ_AT_RESPONSE == msg->kind()) 
        {
            vector<FilePageId> readPages;
            populateData(readPages);
        }
        else if (SPFS_MPI_FILE_READ_RESPONSE == msg->kind())
        {
            vector<FilePageId> readPages;
            populateData(readPages);
        }
        else
        {
            send(msg, appOutGateId());
        }
    }
}

bool DirectPagedMiddlewareCache::lookupData(
    const vector<FilePageId>& requestPageIds)
{
    return true;
}

void DirectPagedMiddlewareCache::populateData(
    const vector<FilePageId>& requestPageIds)
{
    //return true;
}



//
// FullyPagedMiddlewareCache Implementation
//
//
//
// OMNet Registriation Method
Define_Module(FullyPagedMiddlewareCache);

FullyPagedMiddlewareCache::FullyPagedMiddlewareCache()
{
}

void FullyPagedMiddlewareCache::handleApplicationMessage(cMessage* msg)
{
    assert(false);
}

void FullyPagedMiddlewareCache::handleFileSystemMessage(cMessage* msg)
{   
    assert(false);
}


//
// CooperativeDirectMiddlewareCache Implementation
//
//
// OMNet Registriation Method
Define_Module(CooperativeDirectMiddlewareCache);

CooperativeDirectMiddlewareCache::CooperativeDirectMiddlewareCache()
{
}

void CooperativeDirectMiddlewareCache::handleApplicationMessage(cMessage* msg)
{
    assert(false);
}

void CooperativeDirectMiddlewareCache::handleFileSystemMessage(cMessage* msg)
{   
    assert(false);
}




/*
 * Local variables:
 *  indent-tabs-mode: nil
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 *
 * vim: ts=8 sts=4 sw=4 expandtab
 */
