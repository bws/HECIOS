#ifndef FS_CLIENT_TEST_H
#define FS_CLIENT_TEST_H
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
#include "basic_data_type.h"
#include "csimple_module_tester.h"
#include "file_builder.h"
#include "file_descriptor.h"
#include "filename.h"
#include "fs_client.h"
#include "mock_storage_layout_manager.h"
#include "mpi_proto_m.h"
using namespace std;

/** Unit test for NoTranslation */
class FSClientTest : public CppUnit::TestFixture
{
    // Create generic unit test and register test functions for automatic
    // exercise
    CPPUNIT_TEST_SUITE(FSClientTest);
    CPPUNIT_TEST(testReadAtNoOffsetNoLength);
    CPPUNIT_TEST_SUITE_END();

public:

    /** Called before each test function */
    virtual void setUp();

    /** Called after each test function */
    virtual void tearDown();

    /** Test an MPIFileReadAt of 0 offset, 0 length */
    void testReadAtNoOffsetNoLength();

private:

    cSimpleModuleTester* moduleTester_;
};

void FSClientTest::setUp()
{
    // Create the module for testing
    moduleTester_ = new cSimpleModuleTester("FSClient",
                                            "src/client/fs_client.ned");
    // Setup data servers
    HandleRange r1 = {0, 99};
    HandleRange r2 = {100, 199};
    int metaServerId = FileBuilder::instance().registerFSServer(r1, true);
    FileBuilder::instance().registerFSServer(r2, false);

    // Create the test file
    Filename testFile("/testFile");
    FSSize size = 9168;
    MockStorageLayoutManager layout;
    FileBuilder::instance().createFile(testFile, size, metaServerId, 2, layout);

}

void FSClientTest::tearDown()
{
    delete moduleTester_;
    moduleTester_ = 0;

    FileBuilder::clearState();
}

// Test a MPIFileReadAt with 0 offset, 0 length
void FSClientTest::testReadAtNoOffsetNoLength()
{
    // Retrieve the test files handle
    BasicDataType<4> dt1;
    Filename testFile("/testFile");
    FileDescriptor* fd = FileBuilder::instance().getDescriptor(testFile);
    spfsMPIFileReadAtRequest* req =
        new spfsMPIFileReadAtRequest(0, SPFS_MPI_FILE_READ_AT_REQUEST);
    req->setFileDes(fd);
    req->setCount(0);
    req->setOffset(0);
    req->setDataType(&dt1);

    // Ensure the output message is a ReadAtResponse
    //moduleTester_->deliverMessage(req, "appIn");

    // TODO: account for the processing delay now encountered here
    //cMessage* output1 = moduleTester_->getOutputMessage();
    //CPPUNIT_ASSERT_EQUAL((size_t)1, moduleTester_->getNumOutputMessages());
    //CPPUNIT_ASSERT(0 != dynamic_cast<spfsMPIFileReadAtResponse*>(output1));

    // Cleanup test data
    delete req;
    delete fd;
}

#endif

/*
 * Local variables:
 *  indent-tabs-mode: nil
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 *
 * vim: ts=8 sts=4 sw=4 expandtab
 */
