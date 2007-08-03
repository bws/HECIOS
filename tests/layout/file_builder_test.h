#ifndef FILE_BUILDER_TEST_H
#define FILE_BUILDER_TEST_H

#include <cstddef>
#include <iostream>
#include <string>
#include <cppunit/extensions/HelperMacros.h>
#include "file_builder.h"
#include "filename.h"
using namespace std;

/** Unit test for FileBuilder */
class FileBuilderTest : public CppUnit::TestFixture
{
    // Create generic unit test and register test functions for automatic
    // exercise
    CPPUNIT_TEST_SUITE(FileBuilderTest);
    CPPUNIT_TEST(testInstance);
    CPPUNIT_TEST(testRegisterFSServer);
    CPPUNIT_TEST(testGetMetaServers);
    CPPUNIT_TEST(testGetNextHandle);
    CPPUNIT_TEST(testFileExists);
    CPPUNIT_TEST(testGetMetaData);
    CPPUNIT_TEST(testGetDescriptor);
    CPPUNIT_TEST(testCreateDirectory);
    CPPUNIT_TEST(testCreateFile);
    CPPUNIT_TEST_SUITE_END();

public:

    /** Called before each test function */
    virtual void setUp();

    /** Called after each test function */
    virtual void tearDown();

    void testInstance();
    void testRegisterFSServer();
    void testGetMetaServers();
    void testGetNextHandle();
    void testFileExists();
    void testGetMetaData();
    void testGetDescriptor();
    void testCreateDirectory();
    void testCreateFile();

private:
    HandleRange range1_;
    HandleRange range2_;
};

void FileBuilderTest::setUp()
{
    // Register servers for use during testing
    range1_.first = 100; range1_.last = 200;
    range2_.first = 2000; range2_.last = 3000;
    FileBuilder::instance().registerFSServer(range1_, true);
    FileBuilder::instance().registerFSServer(range2_, false);
}

void FileBuilderTest::tearDown()
{
    // Clear the singleton state between tests
    FileBuilder::clearState();
}

void FileBuilderTest::testInstance()
{
    FileBuilder& utils = FileBuilder::instance();
    CPPUNIT_ASSERT(0 != &utils);
}

void FileBuilderTest::testRegisterFSServer()
{
    HandleRange h1, h2;
    FileBuilder::instance().registerFSServer(h1, true);
    FileBuilder::instance().registerFSServer(h2, false);
    CPPUNIT_ASSERT(true);
}

void FileBuilderTest::testGetMetaServers()
{
    // Clear the setup state to ease testing
    FileBuilder::clearState();

    // Register 3 servers with servers 1 and 3 set as Meta servers
    HandleRange h1, h2, h3;
    FileBuilder::instance().registerFSServer(h1, true);
    FileBuilder::instance().registerFSServer(h2, false);
    FileBuilder::instance().registerFSServer(h3, true);
    vector<int> metaServers = FileBuilder::instance().getMetaServers();
    CPPUNIT_ASSERT_EQUAL(0, metaServers[0]);
    CPPUNIT_ASSERT_EQUAL(2, metaServers[1]);
}

void FileBuilderTest::testGetNextHandle()
{
    CPPUNIT_ASSERT_EQUAL(range1_.first, FileBuilder::instance().getNextHandle(0));
    CPPUNIT_ASSERT_EQUAL(range1_.first + 1,
                         FileBuilder::instance().getNextHandle(0));
    CPPUNIT_ASSERT_EQUAL(range2_.first, FileBuilder::instance().getNextHandle(1));
    CPPUNIT_ASSERT_EQUAL(range2_.first + 1,
                         FileBuilder::instance().getNextHandle(1));
}

void FileBuilderTest::testFileExists()
{
    // Create files
    string file1 = "/test1";
    string file2 = "/test2";
    string dir1 = "/dir1";
    FileBuilder::instance().createFile(file1, 0, 1);
    FileBuilder::instance().createDirectory(dir1, 0);
    CPPUNIT_ASSERT(FileBuilder::instance().fileExists(file1));
    CPPUNIT_ASSERT(FileBuilder::instance().fileExists(dir1));
    CPPUNIT_ASSERT(!FileBuilder::instance().fileExists(file2));
}

void FileBuilderTest::testGetMetaData()
{
    Filename file1("/test1");
    FileBuilder::instance().createFile(file1, 0, 1);
    FSMetaData* file1MD = FileBuilder::instance().getMetaData(file1);
    CPPUNIT_ASSERT(0 != file1MD);
    
    string dir1 = "/dir1";
    FileBuilder::instance().createDirectory(dir1, 0);
    //CPPUNIT_FAIL("Not implemented");
}

void FileBuilderTest::testGetDescriptor()
{
    //CPPUNIT_FAIL("Not implemented");
}

void FileBuilderTest::testCreateDirectory()
{
    Filename dir1("/dir1");
    Filename dir2("/");
    Filename dir3("/foo/bar/baz");
    FileBuilder::instance().createDirectory(dir1, 0);
    FileBuilder::instance().createDirectory(dir2, 0);
    FileBuilder::instance().createDirectory(dir3, 0);

    CPPUNIT_ASSERT(FileBuilder::instance().fileExists(dir1));
    CPPUNIT_ASSERT(FileBuilder::instance().fileExists(dir2));
    CPPUNIT_ASSERT(FileBuilder::instance().fileExists(Filename("/")));
    CPPUNIT_ASSERT(FileBuilder::instance().fileExists(Filename("/foo")));
    CPPUNIT_ASSERT(FileBuilder::instance().fileExists(Filename("/foo/bar")));
    CPPUNIT_ASSERT(FileBuilder::instance().fileExists(Filename("/foo/bar/baz")));
}

void FileBuilderTest::testCreateFile()
{
    Filename file1("/file1");
    Filename file2("/foo/bar/baz");
    FileBuilder::instance().createFile(file1, 0, 1);
    FileBuilder::instance().createFile(file2, 0, 1);

    CPPUNIT_ASSERT(FileBuilder::instance().fileExists(file1));
    CPPUNIT_ASSERT(FileBuilder::instance().fileExists(Filename("/")));
    CPPUNIT_ASSERT(FileBuilder::instance().fileExists(Filename("/foo")));
    CPPUNIT_ASSERT(FileBuilder::instance().fileExists(Filename("/foo/bar")));
    CPPUNIT_ASSERT(FileBuilder::instance().fileExists(Filename("/foo/bar/baz")));
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