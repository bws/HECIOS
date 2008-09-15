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
    CPPUNIT_TEST(testApplicationRead);
    CPPUNIT_TEST(testWrite);
    CPPUNIT_TEST(testWriteAt);
    CPPUNIT_TEST_SUITE_END();

public:

    /** Called before each test function */
    virtual void setUp();

    /** Called after each test function */
    virtual void tearDown();

    /** Test components of a ReadAt request */
    void testApplicationRead();

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
    FileBuilder::instance().createFile(*file1_, 1024, 0, 4, layout);
    file2_ = new Filename("/file2");
    FileBuilder::instance().createFile(*file2_, 1024, 0, 4, layout);
    file3_ = new Filename("/file3");
    FileBuilder::instance().createFile(*file3_, 1024, 0, 4, layout);
    file4_ = new Filename("/file4");
    FileBuilder::instance().createFile(*file4_, 1024, 0, 4, layout);

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

void DirectPagedMiddlewareCacheTest::testApplicationRead()
{
    // Data for use in tests
    ByteDataType byteDataType;
    VectorDataType vectorDataType();

    //
    // Test Case 1
    //
    spfsMPIFileReadAtRequest* readAt =
        new spfsMPIFileReadAtRequest(0, SPFS_MPI_FILE_READ_AT_REQUEST);
    readAt->setFileDes(FileBuilder::instance().getDescriptor(*file1_));
    readAt->setDataType(&byteDataType);
    readAt->setCount(4000);
    readAt->setOffset(0);
    moduleTester_->deliverMessage(readAt, "appIn");

    size_t numOutput = moduleTester_->getNumOutputMessages();
    CPPUNIT_ASSERT_EQUAL(size_t(40), numOutput);

    delete readAt;
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
