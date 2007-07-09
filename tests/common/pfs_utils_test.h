#ifndef PFS_UTILS_TEST_H
#define PFS_UTILS_TEST_H

#include <cstddef>
#include <iostream>
#include <string>
#include <cppunit/extensions/HelperMacros.h>
#include "IPvXAddress.h"
#include "pfs_utils.h"
using namespace std;

/** Unit test for PFSUtils */
class PFSUtilsTest : public CppUnit::TestFixture
{
    // Create generic unit test and register test functions for automatic
    // exercise
    CPPUNIT_TEST_SUITE(PFSUtilsTest);
    CPPUNIT_TEST(testInstance);
    CPPUNIT_TEST(testRegisterServerIP);
    CPPUNIT_TEST(testGetServerIP);
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
    void testRegisterServerIP();
    void testGetServerIP();
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

void PFSUtilsTest::setUp()
{
    // Register servers for use during testing
    range1_.first = 100; range1_.last = 200;
    range2_.first = 2000; range2_.last = 3000;
    PFSUtils::instance().registerFSServer(range1_, true);
    PFSUtils::instance().registerFSServer(range2_, false);
}

void PFSUtilsTest::tearDown()
{
    // Clear the singleton state between tests
    PFSUtils::clearState();
}

void PFSUtilsTest::testInstance()
{
    PFSUtils& utils = PFSUtils::instance();
    CPPUNIT_ASSERT(0 != &utils);
}

void PFSUtilsTest::testRegisterServerIP()
{
    // Clear the setup state to ease testing
    PFSUtils::clearState();

    // test for single addition
    IPvXAddress* ip1 = new IPvXAddress("192.168.0.1");
    HandleRange r1;
    r1.first = 1;
    r1.last = 10;
    FSHandle h1_1(1);
    FSHandle h1_2(2);
    FSHandle h1_3(10);

    PFSUtils::instance().registerServerIP(ip1, r1);
    CPPUNIT_ASSERT_EQUAL(ip1 , PFSUtils::instance().getServerIP(h1_1));
    CPPUNIT_ASSERT_EQUAL(ip1 , PFSUtils::instance().getServerIP(h1_2));
    CPPUNIT_ASSERT_EQUAL(ip1 , PFSUtils::instance().getServerIP(h1_3));

    // test for multiple additions
    IPvXAddress* ip2 = new IPvXAddress("192.168.0.2");
    IPvXAddress* ip3 = new IPvXAddress("192.168.0.3");
    HandleRange r2;
    r2.first = 11;
    r2.last  = 20;
    HandleRange r3;
    r3.first = 21;
    r3.last  = 30;
    FSHandle h2_1(15);
    FSHandle h3_1(25);
    
    PFSUtils::instance().registerServerIP(ip2, r2);
    PFSUtils::instance().registerServerIP(ip3, r3);
    CPPUNIT_ASSERT_EQUAL(ip2 , PFSUtils::instance().getServerIP(h2_1));
    CPPUNIT_ASSERT_EQUAL(ip3 , PFSUtils::instance().getServerIP(h3_1));

    // test for duplicate additions
    IPvXAddress* ip4 = new IPvXAddress("192.168.0.4");
    HandleRange r4;
    r4.first = 31;
    r4.last  = 40;
    FSHandle h4(35);

    PFSUtils::instance().registerServerIP(ip4, r4);
    PFSUtils::instance().registerServerIP(ip4, r4);
    CPPUNIT_ASSERT_EQUAL(ip4 , PFSUtils::instance().getServerIP(h4));

    // test for overwrite
    IPvXAddress* ip5_1 = new IPvXAddress("192.168.0.5");
    IPvXAddress* ip5_2 = new IPvXAddress("192.168.0.6");
    HandleRange r5;
    r5.first = 41;
    r5.last  = 50;
    FSHandle h5(45);

    PFSUtils::instance().registerServerIP(ip5_1, r5);
    PFSUtils::instance().registerServerIP(ip5_2, r5);
    CPPUNIT_ASSERT_EQUAL(ip5_2 , PFSUtils::instance().getServerIP(h5));
}

void PFSUtilsTest::testGetServerIP()
{
    // Clear the setup state to ease testing
    PFSUtils::clearState();

    // test for handle in and outside the handle-ranges of map
    IPvXAddress* ip7 = new IPvXAddress("192.168.0.7");
    HandleRange r7;
    r7.first = 60;
    r7.last = 70;
    FSHandle h7_1(1000);
    FSHandle h7_2(55);
    FSHandle h7_3(0);
    FSHandle h7_4(60);
    
    PFSUtils::instance().registerServerIP(ip7, r7);
    CPPUNIT_ASSERT(0 == PFSUtils::instance().getServerIP(h7_1));
    CPPUNIT_ASSERT(0 == PFSUtils::instance().getServerIP(h7_2));
    CPPUNIT_ASSERT(0 == PFSUtils::instance().getServerIP(h7_3));
    CPPUNIT_ASSERT_EQUAL(ip7 , PFSUtils::instance().getServerIP(h7_4));

}

void PFSUtilsTest::testRegisterFSServer()
{
    HandleRange h1, h2;
    PFSUtils::instance().registerFSServer(h1, true);
    PFSUtils::instance().registerFSServer(h2, false);
    CPPUNIT_ASSERT(true);
}

void PFSUtilsTest::testGetMetaServers()
{
    // Clear the setup state to ease testing
    PFSUtils::clearState();

    // Register 3 servers with servers 1 and 3 set as Meta servers
    HandleRange h1, h2, h3;
    PFSUtils::instance().registerFSServer(h1, true);
    PFSUtils::instance().registerFSServer(h2, false);
    PFSUtils::instance().registerFSServer(h3, true);
    vector<int> metaServers = PFSUtils::instance().getMetaServers();
    CPPUNIT_ASSERT_EQUAL(0, metaServers[0]);
    CPPUNIT_ASSERT_EQUAL(2, metaServers[1]);
}

void PFSUtilsTest::testGetNextHandle()
{
    CPPUNIT_ASSERT_EQUAL(range1_.first, PFSUtils::instance().getNextHandle(0));
    CPPUNIT_ASSERT_EQUAL(range1_.first + 1,
                         PFSUtils::instance().getNextHandle(0));
    CPPUNIT_ASSERT_EQUAL(range2_.first, PFSUtils::instance().getNextHandle(1));
    CPPUNIT_ASSERT_EQUAL(range2_.first + 1,
                         PFSUtils::instance().getNextHandle(1));
}

void PFSUtilsTest::testFileExists()
{
    // Create files
    string file1 = "/test1";
    string file2 = "/test2";
    string dir1 = "/dir1";
    PFSUtils::instance().createFile(file1, 0, 1);
    PFSUtils::instance().createDirectory(dir1, 0);
    CPPUNIT_ASSERT(PFSUtils::instance().fileExists(file1));
    CPPUNIT_ASSERT(PFSUtils::instance().fileExists(dir1));
    CPPUNIT_ASSERT(!PFSUtils::instance().fileExists(file2));
}

void PFSUtilsTest::testGetMetaData()
{
    Filename file1("/test1");
    PFSUtils::instance().createFile(file1, 0, 1);
    FSMetaData* file1MD = PFSUtils::instance().getMetaData(file1);
    CPPUNIT_ASSERT(0 != file1MD);
    
    string dir1 = "/dir1";
    PFSUtils::instance().createDirectory(dir1, 0);
    //CPPUNIT_FAIL("Not implemented");
}

void PFSUtilsTest::testGetDescriptor()
{
    //CPPUNIT_FAIL("Not implemented");
}

void PFSUtilsTest::testCreateDirectory()
{
    Filename dir1("/dir1");
    Filename dir2("/");
    Filename dir3("/foo/bar/baz");
    PFSUtils::instance().createDirectory(dir1, 0);
    PFSUtils::instance().createDirectory(dir2, 0);
    PFSUtils::instance().createDirectory(dir3, 0);

    CPPUNIT_ASSERT(PFSUtils::instance().fileExists(dir1));
    CPPUNIT_ASSERT(PFSUtils::instance().fileExists(dir2));
    CPPUNIT_ASSERT(PFSUtils::instance().fileExists(Filename("/")));
    CPPUNIT_ASSERT(PFSUtils::instance().fileExists(Filename("/foo")));
    CPPUNIT_ASSERT(PFSUtils::instance().fileExists(Filename("/foo/bar")));
    CPPUNIT_ASSERT(PFSUtils::instance().fileExists(Filename("/foo/bar/baz")));
}

void PFSUtilsTest::testCreateFile()
{
    Filename file1("/file1");
    Filename file2("/foo/bar/baz");
    PFSUtils::instance().createFile(file1, 0, 1);
    PFSUtils::instance().createFile(file2, 0, 1);

    CPPUNIT_ASSERT(PFSUtils::instance().fileExists(file1));
    CPPUNIT_ASSERT(PFSUtils::instance().fileExists(Filename("/")));
    CPPUNIT_ASSERT(PFSUtils::instance().fileExists(Filename("/foo")));
    CPPUNIT_ASSERT(PFSUtils::instance().fileExists(Filename("/foo/bar")));
    CPPUNIT_ASSERT(PFSUtils::instance().fileExists(Filename("/foo/bar/baz")));
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
