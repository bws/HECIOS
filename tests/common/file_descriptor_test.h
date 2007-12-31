#ifndef FILE_DESCRIPTOR_TEST_H
#define FILE_DESCRIPTOR_TEST_H
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

#include <iostream>
#include <string>
#include <cppunit/TestAssert.h>
#include <cppunit/extensions/HelperMacros.h>
#include "file_builder.h"
#include "filename.h"
#include "mock_storage_layout_manager.h"
#include "file_descriptor.h"
using namespace std;

/** Unit test for FileDescriptor */
class FileDescriptorTest : public CppUnit::TestFixture
{
    // Create generic unit test and register test functions for automatic
    // exercise
    CPPUNIT_TEST_SUITE(FileDescriptorTest);
    CPPUNIT_TEST(testConstructor);
    CPPUNIT_TEST(testGetFilename);
    CPPUNIT_TEST(testGetFilePointer);
    CPPUNIT_TEST(testGetHandle);
    CPPUNIT_TEST(testGetMetaData);
    CPPUNIT_TEST(testGetNumParentHandles);
    CPPUNIT_TEST(testGetParentHandle);
    CPPUNIT_TEST(testSetFileView);
    CPPUNIT_TEST(testSetFilePointer);
    CPPUNIT_TEST(testMoveFilePointer);
    CPPUNIT_TEST_SUITE_END();

public:
    /** Called before each test function */
    void setUp();

    /** Called after each test function */
    void tearDown();
    
    void testConstructor();

    void testGetFilename();

    void testGetFilePointer();

    void testGetHandle();

    void testGetMetaData();

    void testGetNumParentHandles();

    void testGetParentHandle();

    void testSetFileView();

    void testSetFilePointer();

    void testMoveFilePointer();
    
private:
    FSMetaData* metaData;
    MockStorageLayoutManager layoutManager;
};

void FileDescriptorTest::setUp()
{
    metaData = new FSMetaData();
    metaData->handle = 1;

    HandleRange range = {0, 1000};
    FileBuilder::instance().registerFSServer(range, true);
    Filename root("/");
    FileBuilder::instance().createDirectory(root, 0, layoutManager); 
}

void FileDescriptorTest::tearDown()
{
    delete metaData;
    metaData = 0;

    FileBuilder::clearState();
}

void FileDescriptorTest::testConstructor()
{
    Filename name("/foo");
    FileDescriptor fd1(name, *metaData);
    CPPUNIT_ASSERT_EQUAL(Filename("/foo"), fd1.getFilename());
    CPPUNIT_ASSERT_EQUAL(FSOffset(0), fd1.getFilePointer());
    CPPUNIT_ASSERT_EQUAL(FSHandle(1), fd1.getHandle());
    CPPUNIT_ASSERT_EQUAL((size_t)1, fd1.getNumParentHandles());
    CPPUNIT_ASSERT_EQUAL(FSHandle(0), fd1.getParentHandle(0));
}

void FileDescriptorTest::testGetFilename()
{
    Filename name1("/foo");
    FileDescriptor fd1(name1, *metaData);
    CPPUNIT_ASSERT_EQUAL(name1, fd1.getFilename());
    
    Filename name2("/popple/foople/foo");
    FileDescriptor fd2(name2, *metaData);
    CPPUNIT_ASSERT_EQUAL(name2, fd2.getFilename());
}

void FileDescriptorTest::testGetFilePointer()
{
    Filename name1("/foo");
    FileDescriptor fd1(name1, *metaData);
    CPPUNIT_ASSERT_EQUAL(FSOffset(0), fd1.getFilePointer());
}

void FileDescriptorTest::testGetHandle()
{
    Filename name1("/foo");
    FileDescriptor fd1(name1, *metaData);
    CPPUNIT_ASSERT_EQUAL(FSHandle(1), fd1.getHandle());
}

void FileDescriptorTest::testGetMetaData()
{
    Filename name1("/foo");
    FileDescriptor fd1(name1, *metaData);
    CPPUNIT_ASSERT_EQUAL(metaData->handle, fd1.getMetaData()->handle);
}

void FileDescriptorTest::testGetNumParentHandles()
{
    Filename name1("/foo/poo/goo");
    FileDescriptor fd1(name1, *metaData);
    CPPUNIT_ASSERT_EQUAL((size_t)1, fd1.getNumParentHandles());
}

void FileDescriptorTest::testGetParentHandle()
{
    Filename name1("/foo/poo/goo");
    FileDescriptor fd1(name1, *metaData);
    CPPUNIT_ASSERT_EQUAL(FSHandle(0), fd1.getParentHandle(0));
}

void FileDescriptorTest::testSetFileView()
{
    Filename name1("/foo/poo/goo");
    FileDescriptor fd1(name1, *metaData);

    FileView view1(11, new BasicDataType(7));
    fd1.setFileView(view1);
    CPPUNIT_ASSERT_EQUAL(FSOffset(11), fd1.getFileView().getDisplacement());
}

void FileDescriptorTest::testSetFilePointer()
{
    Filename name1("/foo/poo/goo");
    FileDescriptor fd1(name1, *metaData);
    fd1.setFilePointer(311);
    CPPUNIT_ASSERT_EQUAL(FSOffset(311), fd1.getFilePointer());
}

void FileDescriptorTest::testMoveFilePointer()
{
    Filename name1("/foo/poo/goo");
    FileDescriptor fd1(name1, *metaData);
    fd1.setFilePointer(100);

    fd1.moveFilePointer(5);
    CPPUNIT_ASSERT_EQUAL(FSOffset(105), fd1.getFilePointer());

    fd1.moveFilePointer(10);
    CPPUNIT_ASSERT_EQUAL(FSOffset(115), fd1.getFilePointer());

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
