#ifndef FS_SERVER_TEST_H
#define FS_SERVER_TEST_H
//
// This file is part of Hecios
//
// Copyright (C) 2007,2008,2009 Brad Settlemyer
//
// This file is distributed WITHOUT ANY WARRANTY. See the file 'License.txt'
// for details on this and other legal matters.
//
#include <cstddef>
#include <iostream>
#include <string>
#include <cppunit/extensions/HelperMacros.h>
#include "csimple_module_tester.h"
#include "file_builder.h"
#include "fs_server.h"
#include "mock_storage_layout_manager.h"
#include "mpi_proto_m.h"
#include "os_proto_m.h"
#include "pvfs_proto_m.h"
using namespace std;

/** Unit test for FSServer */
class FSServerTest : public CppUnit::TestFixture
{
    // Create generic unit test and register test functions for automatic
    // exercise
    CPPUNIT_TEST_SUITE(FSServerTest);
    CPPUNIT_TEST(testGetAttr);
    CPPUNIT_TEST(testSetAttr);
    CPPUNIT_TEST_SUITE_END();

public:

    /** Called before each test function */
    virtual void setUp();

    /** Called after each test function */
    virtual void tearDown();

    /** Test components of a GetAttr request */
    void testGetAttr();

    /** Test components of a SetAttr request */
    void testSetAttr();

private:

    cSimpleModuleTester* moduleTester_;
    FSMetaData* testMeta_;
};

void FSServerTest::setUp()
{
    // Register servers for use during testing
    HandleRange range1 = {100, 200};
    HandleRange range2 = {2000, 3000};
    FileBuilder::instance().registerFSServer(range1, true);
    FileBuilder::instance().registerFSServer(range2, false);

    // Test file for use during testing
    MockStorageLayoutManager layout;
    Filename file("/dir1/dir2/file1");
    FileBuilder::instance().createFile(file, 1024, 0, 4, layout);
    testMeta_ = FileBuilder::instance().getMetaData(file);
    
    // Create the module for testing
    moduleTester_ = new cSimpleModuleTester("FSServer",
                                            "src/server/fs_server.ned");
}

void FSServerTest::tearDown()
{
    FileBuilder::clearState();
    
    delete moduleTester_;
    moduleTester_ = 0;
}

// Test a GetAttr request
void FSServerTest::testGetAttr()
{
    // Test the delivery of the first request
    spfsMPIFileReadRequest mpiRequest(0, SPFS_MPI_FILE_READ_REQUEST);
    spfsGetAttrRequest getAttrRequest(0, SPFS_GET_ATTR_REQUEST);
    getAttrRequest.setHandle(testMeta_->handle);
    getAttrRequest.setContextPointer(&mpiRequest);
    moduleTester_->deliverMessage(&getAttrRequest, "in");
    CPPUNIT_ASSERT_EQUAL((size_t)1, moduleTester_->getNumOutputMessages());
    cMessage* out1 = moduleTester_->popOutputMessage();
    CPPUNIT_ASSERT(0 != dynamic_cast<spfsOSFileReadRequest*>(out1));

    // Test the delivery of the OS response
    spfsOSFileReadResponse* readResponse =
        new spfsOSFileReadResponse(0, SPFS_OS_FILE_READ_RESPONSE);
    readResponse->setContextPointer(out1);
    moduleTester_->deliverMessage(readResponse, "in");
    cMessage* out2 = moduleTester_->getOutputMessage();
    CPPUNIT_ASSERT(0 != dynamic_cast<spfsGetAttrResponse*>(out2));
}

// Test a SetAttr request
void FSServerTest::testSetAttr()
{
    // Test the delivery of the first request
    spfsMPIFileOpenRequest mpiRequest(0, SPFS_MPI_FILE_OPEN_REQUEST);
    spfsSetAttrRequest setAttrRequest(0, SPFS_SET_ATTR_REQUEST);
    setAttrRequest.setHandle(testMeta_->handle);
    setAttrRequest.setContextPointer(&mpiRequest);
    moduleTester_->deliverMessage(&setAttrRequest, "in");
    CPPUNIT_ASSERT_EQUAL((size_t)1, moduleTester_->getNumOutputMessages());
    cMessage* out1 = moduleTester_->popOutputMessage();
    CPPUNIT_ASSERT(0 != dynamic_cast<spfsOSFileWriteRequest*>(out1));

    // Test the delivery of the OS response
    spfsOSFileWriteResponse* writeResponse =
        new spfsOSFileWriteResponse(0, SPFS_OS_FILE_WRITE_RESPONSE);
    writeResponse->setContextPointer(out1);
    moduleTester_->deliverMessage(writeResponse, "in");
    cMessage* out2 = moduleTester_->getOutputMessage();
    CPPUNIT_ASSERT(0 != dynamic_cast<spfsSetAttrResponse*>(out2));

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
