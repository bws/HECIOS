#ifndef NATIVE_FILE_SYSTEM_TEST_H
#define NATIVE_FILE_SYSTEM_TEST_H
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
#include "filename.h"
#include "file_system.h"
#include "mpi_proto_m.h"
#include "os_proto_m.h"
#include "pvfs_proto_m.h"
using namespace std;

/** Unit test for the NativeFileSystem module */
class NativeFileSystemTest : public CppUnit::TestFixture
{
    // Create generic unit test and register test functions for automatic
    // exercise
    CPPUNIT_TEST_SUITE(NativeFileSystemTest);
    CPPUNIT_TEST(testFileRead);
    CPPUNIT_TEST(testFileWrite);
    CPPUNIT_TEST_SUITE_END();

public:

    /** Called before each test function */
    virtual void setUp();

    /** Called after each test function */
    virtual void tearDown();

    /** Test components of a OSFileRead request */
    void testFileRead();

    /** Test components of a OSFileWrite request */
    void testFileWrite();

private:

    cSimpleModuleTester* moduleTester_;
};

void NativeFileSystemTest::setUp()
{
    // Create the module for testing
    moduleTester_ = new cSimpleModuleTester("NativeFileSystem",
                                            "src/os/file_system.ned",
                                            false);

    // Set the block size for the file system
    cSimpleModule* fs = moduleTester_->getModule();
    fs->par("blockSizeBytes") = 256;
    moduleTester_->callInitialize();
}

void NativeFileSystemTest::tearDown()
{
    // Cleanup module
    delete moduleTester_;
    moduleTester_ = 0;
}

// Test an OSFileRead request
void NativeFileSystemTest::testFileRead()
{
    // Allocate the file system storage manually
    Filename testFile("/testFile");
    FileSystem* fs = (FileSystem*)moduleTester_->getModule();
    fs->createFile(testFile, 10000);

    // Send the OSFileRead request
    spfsOSFileReadRequest fileRead(0, SPFS_OS_FILE_READ_REQUEST);
    fileRead.setFilename(testFile.c_str());
    fileRead.setOffset(12);
    fileRead.setExtent(24);
    moduleTester_->deliverMessage(&fileRead, "in");

    // Test that file system reads meta data
    CPPUNIT_ASSERT_EQUAL(1u, moduleTester_->getNumOutputMessages());
    cMessage* out1 = moduleTester_->popOutputMessage();
    CPPUNIT_ASSERT(0 != dynamic_cast<spfsOSReadBlocksRequest*>(out1));

    // Send the read meta data response
    spfsOSReadBlocksResponse* readMetaResponse =
        new spfsOSReadBlocksResponse(0, SPFS_OS_READ_BLOCKS_RESPONSE);
    readMetaResponse->setContextPointer(out1);
    moduleTester_->deliverMessage(readMetaResponse, "response");
    CPPUNIT_ASSERT_EQUAL(2u, moduleTester_->getNumOutputMessages());

    // Test that file system sends meta update and data read
    cMessage* out2 = moduleTester_->popOutputMessage();
    cMessage* out3 = moduleTester_->popOutputMessage();
    CPPUNIT_ASSERT(0 != dynamic_cast<spfsOSWriteBlocksRequest*>(out2));
    CPPUNIT_ASSERT(0 != dynamic_cast<spfsOSReadBlocksRequest*>(out3));
    
    // Send the write meta data response
    spfsOSWriteBlocksResponse* writeMetaResponse =
        new spfsOSWriteBlocksResponse(0, SPFS_OS_WRITE_BLOCKS_RESPONSE);
    writeMetaResponse->setContextPointer(out2);
    moduleTester_->deliverMessage(writeMetaResponse, "response");
    CPPUNIT_ASSERT_EQUAL(0u, moduleTester_->getNumOutputMessages());
    
    // Send the read data response
    spfsOSReadBlocksResponse* readDataResponse =
        new spfsOSReadBlocksResponse(0, SPFS_OS_READ_BLOCKS_RESPONSE);
    readDataResponse->setContextPointer(out3);
    moduleTester_->deliverMessage(readDataResponse, "response");

    // Test that final response is sent
    CPPUNIT_ASSERT_EQUAL(1u, moduleTester_->getNumOutputMessages());
    cMessage* out4 = moduleTester_->getOutputMessage();
    CPPUNIT_ASSERT(0 != dynamic_cast<spfsOSFileReadResponse*>(out4));    
}

// Test an OSFileWrite request
void NativeFileSystemTest::testFileWrite()
{
    // Allocate the file system storage manually
    Filename testFile("/testFile");
    FileSystem* fs = (FileSystem*)moduleTester_->getModule();
    fs->createFile(testFile, 10000);

    // Send the OSFileRead request
    spfsOSFileWriteRequest fileWrite(0, SPFS_OS_FILE_WRITE_REQUEST);
    fileWrite.setFilename(testFile.c_str());
    fileWrite.setOffset(12);
    fileWrite.setExtent(24);
    moduleTester_->deliverMessage(&fileWrite, "in");

    // Test that file system reads meta data
    CPPUNIT_ASSERT_EQUAL(1u, moduleTester_->getNumOutputMessages());
    cMessage* out1 = moduleTester_->popOutputMessage();
    CPPUNIT_ASSERT(0 != dynamic_cast<spfsOSReadBlocksRequest*>(out1));

    // Send the read meta data response
    spfsOSReadBlocksResponse* readMetaResponse =
        new spfsOSReadBlocksResponse(0, SPFS_OS_READ_BLOCKS_RESPONSE);
    readMetaResponse->setContextPointer(out1);
    moduleTester_->deliverMessage(readMetaResponse, "response");
    CPPUNIT_ASSERT_EQUAL(2u, moduleTester_->getNumOutputMessages());

    // Test that file system sends meta update and data read
    cMessage* out2 = moduleTester_->popOutputMessage(); // Metadata
    cMessage* out3 = moduleTester_->popOutputMessage(); // Data
    CPPUNIT_ASSERT(0 != dynamic_cast<spfsOSWriteBlocksRequest*>(out2));
    CPPUNIT_ASSERT(0 != dynamic_cast<spfsOSWriteBlocksRequest*>(out3));
    
    // Send the write data response
    spfsOSWriteBlocksResponse* writeDataResponse =
        new spfsOSWriteBlocksResponse(0, SPFS_OS_WRITE_BLOCKS_RESPONSE);
    writeDataResponse->setContextPointer(out3);
    moduleTester_->deliverMessage(writeDataResponse, "response");
    CPPUNIT_ASSERT_EQUAL(0u, moduleTester_->getNumOutputMessages());

    // Send the write meta data response
    spfsOSWriteBlocksResponse* writeMetaResponse =
        new spfsOSWriteBlocksResponse(0, SPFS_OS_WRITE_BLOCKS_RESPONSE);
    writeMetaResponse->setContextPointer(out2);
    moduleTester_->deliverMessage(writeMetaResponse, "response");
    
    // Test that final response is sent
    CPPUNIT_ASSERT_EQUAL(1u, moduleTester_->getNumOutputMessages());
    cMessage* out4 = moduleTester_->getOutputMessage();
    CPPUNIT_ASSERT(0 != dynamic_cast<spfsOSFileWriteResponse*>(out4));    
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
