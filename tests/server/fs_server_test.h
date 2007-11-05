#ifndef FS_SERVER_TEST_H
#define FS_SERVER_TEST_H
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

#include <cstddef>
#include <iostream>
#include <string>
#include <cppunit/extensions/HelperMacros.h>
#include "csimple_module_tester.h"
#include "fs_server.h"
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
};

void FSServerTest::setUp()
{
    // Create the module for testing
    moduleTester_ = new cSimpleModuleTester("FSServer",
                                            "src/server/fs_server.ned");
}

void FSServerTest::tearDown()
{
    delete moduleTester_;
    moduleTester_ = 0;
}

// Test a GetAttr request
void FSServerTest::testGetAttr()
{
    // Test the delivery of the first request
    spfsMPIFileReadRequest mpiRequest(0, SPFS_MPI_FILE_READ_REQUEST);
    spfsGetAttrRequest getAttrRequest(0, SPFS_GET_ATTR_REQUEST);
    getAttrRequest.setHandle(1);
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
    setAttrRequest.setHandle(1);
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
