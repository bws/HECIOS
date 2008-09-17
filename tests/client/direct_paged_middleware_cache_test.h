#ifndef DIRECT_PAGED_MIDDLEWARE_CACHE_TEST_H
#define DIRECT_PAGED_MIDDLEWARE_CACHE_TEST_H
//
// This file is part of Hecios
//
// Copyright (C) 2008 Brad Settlemyer
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
#include <cstddef>
#include <iostream>
#include <string>
#include <cppunit/extensions/HelperMacros.h>
#include "basic_data_type.h"
#include "csimple_module_tester.h"
#include "file_builder.h"
#include "file_view.h"
#include "middleware_cache.h"
#include "mock_storage_layout_manager.h"
#include "mpi_proto_m.h"
#include "pvfs_proto_m.h"
#include "vector_data_type.h"
using namespace std;

/** Unit test for DirectPagedMiddlewareCache */
class DirectPagedMiddlewareCacheTest : public CppUnit::TestFixture
{
    // Create generic unit test and register test functions for automatic
    // exercise
    CPPUNIT_TEST_SUITE(DirectPagedMiddlewareCacheTest);
    CPPUNIT_TEST(testApplicationReadContig);
    CPPUNIT_TEST(testApplicationReadVector);
    CPPUNIT_TEST(testWrite);
    CPPUNIT_TEST(testWriteAt);
    CPPUNIT_TEST_SUITE_END();

public:

    /** Called before each test function */
    virtual void setUp();

    /** Called after each test function */
    virtual void tearDown();

    /** Test components of a ReadAt request */
    void testApplicationReadContig();

    /** Test components of a ReadAt request */
    void testApplicationReadVector();

    /** Test components of a Write request */
    void testWrite();

    /** Test components of a WriteAt request */
    void testWriteAt();

private:

    cSimpleModuleTester* moduleTester_;
    Filename* file1_;
    Filename* file2_;
    Filename* file3_;
    Filename* file4_;
};

void DirectPagedMiddlewareCacheTest::setUp()
{
    // Register servers for use during testing
    HandleRange range1 = {100, 200};
    HandleRange range2 = {2000, 3000};
    FileBuilder::instance().registerFSServer(range1, true);
    FileBuilder::instance().registerFSServer(range2, false);

    // Test file for use during testing
    MockStorageLayoutManager layout;
    file1_= new Filename("/file1");
    FileBuilder::instance().createFile(*file1_, 10000, 0, 2, layout);
    file2_ = new Filename("/file2");
    FileBuilder::instance().createFile(*file2_, 10000, 0, 2, layout);
    file3_ = new Filename("/file3");
    FileBuilder::instance().createFile(*file3_, 10000, 0, 2, layout);
    file4_ = new Filename("/file4");
    FileBuilder::instance().createFile(*file4_, 10000, 0, 2, layout);

    // Create the module for testing
    moduleTester_ = new cSimpleModuleTester("DirectPagedMiddlewareCache",
                                            "src/client/middleware_cache.ned",
                                            false);

    // Set the module parameters and initialize
    cModule* module = moduleTester_->getModule();
    module->par("pageSize") = 100;
    module->par("pageCapacity") = 10;
    moduleTester_->callInitialize();

}

void DirectPagedMiddlewareCacheTest::tearDown()
{
    FileBuilder::clearState();

    delete file1_;
    file1_ = 0;
    delete file2_;
    file2_ = 0;
    delete file3_;
    file3_ = 0;
    delete file4_;
    file4_ = 0;

    delete moduleTester_;
    moduleTester_ = 0;
}

void DirectPagedMiddlewareCacheTest::testApplicationReadContig()
{
    //
    // Test Case 1
    //
    ByteDataType byteDataType;
    spfsMPIFileReadAtRequest* appRead =
        new spfsMPIFileReadAtRequest(0, SPFS_MPI_FILE_READ_AT_REQUEST);
    appRead->setFileDes(FileBuilder::instance().getDescriptor(*file1_));
    appRead->setDataType(&byteDataType);
    appRead->setCount(4000);
    appRead->setOffset(0);
    moduleTester_->deliverMessage(appRead, "appIn");

    // Check the number of cache output messages
    size_t numOutput = moduleTester_->getNumOutputMessages();
    CPPUNIT_ASSERT_EQUAL(size_t(1), numOutput);

    // Extract the cache read file view to check it for correctness
    cMessage* outputMsg = moduleTester_->popOutputMessage();
    CPPUNIT_ASSERT(0 != outputMsg);

    // Verify the request
    spfsMPIFileReadAtRequest* cacheRead = dynamic_cast<spfsMPIFileReadAtRequest*>(outputMsg);
    CPPUNIT_ASSERT(0 != cacheRead);
    CPPUNIT_ASSERT_EQUAL(0l, cacheRead->getOffset());
    CPPUNIT_ASSERT_EQUAL(4000, cacheRead->getCount());
    CPPUNIT_ASSERT_EQUAL(byteDataType.getExtent(), cacheRead->getDataType()->getExtent());
    CPPUNIT_ASSERT_EQUAL(byteDataType.getTrueExtent(), cacheRead->getDataType()->getTrueExtent());

    // Verify the applied file view is correct
    FileDescriptor* fd = cacheRead->getFileDes();
    CPPUNIT_ASSERT(0 != fd);

    // Verify the view data type describes the correct file regions
    FileView cacheView = fd->getFileView();
    const DataType* dtype = cacheView.getDataType();
    CPPUNIT_ASSERT_EQUAL(FSOffset(0), cacheView.getDisplacement());
    CPPUNIT_ASSERT(0 != dtype);
    CPPUNIT_ASSERT_EQUAL(size_t(4000), dtype->getExtent());
    CPPUNIT_ASSERT_EQUAL(size_t(4000), dtype->getTrueExtent());

    vector<FileRegion> regions = dtype->getRegionsByBytes(0, 4000);
    CPPUNIT_ASSERT_EQUAL(size_t(40), regions.size());
    for (size_t i = 0; i < regions.size(); i++)
    {
        CPPUNIT_ASSERT_EQUAL(FSOffset(i * 100), regions[i].offset);
        CPPUNIT_ASSERT_EQUAL(FSSize(100), regions[i].extent);
    }

    //
    // Test 2 - Deliver the file system read response
    //
    spfsMPIFileReadAtResponse* cacheReadResponse =
        new spfsMPIFileReadAtResponse(0, SPFS_MPI_FILE_READ_AT_RESPONSE);
    cacheReadResponse->setContextPointer(cacheRead);
    moduleTester_->deliverMessage(cacheReadResponse, "fsIn");

    // Check the number of cache output messages
    CPPUNIT_ASSERT_EQUAL(size_t(1), moduleTester_->getNumOutputMessages());

    // Extract the cache read file view to check it for correctness
    outputMsg = moduleTester_->popOutputMessage();
    CPPUNIT_ASSERT(0 != outputMsg);
    spfsMPIFileReadAtResponse* appReadResponse =
        dynamic_cast<spfsMPIFileReadAtResponse*>(outputMsg);
    CPPUNIT_ASSERT(0 != appReadResponse);
    CPPUNIT_ASSERT(appReadResponse->contextPointer() == appRead);

     // Clean up test 1 and 2 requests
    delete appRead;
    delete appReadResponse;
    delete cacheRead;
    delete cacheReadResponse;

    //
    // Test 3 - Perform a request that is fully cached
    //
    appRead = new spfsMPIFileReadAtRequest(0, SPFS_MPI_FILE_READ_AT_REQUEST);
    appRead->setFileDes(FileBuilder::instance().getDescriptor(*file1_));
    appRead->setDataType(&byteDataType);
    appRead->setCount(300);
    appRead->setOffset(3700);
    moduleTester_->deliverMessage(appRead, "appIn");

    // Check the number of cache output messages
    //CPPUNIT_ASSERT_EQUAL(size_t(0), moduleTester_->getNumOutputMessages());

    // Clean up test
    //delete appRead;
    //delete cacheRead;

}

void DirectPagedMiddlewareCacheTest::testApplicationReadVector()
{
}

void DirectPagedMiddlewareCacheTest::testWrite()
{
}

void DirectPagedMiddlewareCacheTest::testWriteAt()
{
}

#endif

/*
 * Local variables:
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 *
 * vim: ts=8 sts=4 sw=4 expandtab
 */
