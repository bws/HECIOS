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
#include "csimple_module_tester.h"
#include "middleware_cache.h"
#include "mock_storage_layout_manager.h"
#include "mpi_proto_m.h"
#include "pvfs_proto_m.h"
using namespace std;

/** Unit test for DirectPagedMiddlewareCache */
class DirectPagedMiddlewareCacheTest : public CppUnit::TestFixture
{
    // Create generic unit test and register test functions for automatic
    // exercise
    CPPUNIT_TEST_SUITE(DirectPagedMiddlewareCacheTest);
    CPPUNIT_TEST(testRead);
    CPPUNIT_TEST(testReadAt);
    CPPUNIT_TEST(testWrite);
    CPPUNIT_TEST(testWriteAt);
    CPPUNIT_TEST_SUITE_END();

public:

    /** Called before each test function */
    virtual void setUp();

    /** Called after each test function */
    virtual void tearDown();

    /** Test components of a Read request */
    void testRead();

    /** Test components of a ReadAt request */
    void testReadAt();

    /** Test components of a Write request */
    void testWrite();

    /** Test components of a WriteAt request */
    void testWriteAt();

private:

    cSimpleModuleTester* moduleTester_;
    FSMetaData* testMeta_;
};

void DirectPagedMiddlewareCacheTest::setUp()
{
    // Register servers for use during testing
    //HandleRange range1 = {100, 200};
    //HandleRange range2 = {2000, 3000};
    //FileBuilder::instance().registerFSServer(range1, true);
    //FileBuilder::instance().registerFSServer(range2, false);

    // Test file for use during testing
    //MockStorageLayoutManager layout;
    //Filename file("/dir1/dir2/file1");
    //FileBuilder::instance().createFile(file, 1024, 0, 4, layout);
    //testMeta_ = FileBuilder::instance().getMetaData(file);
    
    // Create the module for testing
    moduleTester_ = new cSimpleModuleTester("DirectPagedMiddlewareCache",
                                            "src/client/middleware_cache.ned");
}

void DirectPagedMiddlewareCacheTest::tearDown()
{
    //FileBuilder::clearState();
    
    delete moduleTester_;
    moduleTester_ = 0;
}

void DirectPagedMiddlewareCacheTest::testRead()
{
}

void DirectPagedMiddlewareCacheTest::testReadAt()
{
    //
    // Test Case 1
    //
    spfsMPIFileReadAtRequest* readAt =
        new spfsMPIFileReadAtRequest(0, SPFS_MPI_FILE_READ_AT_REQUEST);
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
