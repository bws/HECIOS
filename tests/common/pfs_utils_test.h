#ifndef PFS_UTILS_TEST_H
#define PFS_UTILS_TEST_H

#include <iostream>
#include <string>
#include <cppunit/extensions/HelperMacros.h>
#include "pfs_utils.h"
using namespace std;

/** Unit test for PFSUtils */
class PFSUtilsTest : public CppUnit::TestFixture
{
    // Create generic unit test and register test functions for automatic
    // exercise
    CPPUNIT_TEST_SUITE(PFSUtilsTest);
    //CPPUNIT_TEST(testConstructor);
    CPPUNIT_TEST(testRegisterServerIP);
    CPPUNIT_TEST(testGetServerIP);
    CPPUNIT_TEST(testInstance);
    CPPUNIT_TEST_SUITE_END();

public:

    /** Called before each test function */
    void setUp();

    /** Called after each test function */
    void tearDown();
    
    void testInstance();
    
    //void testConstructor();
    void testRegisterServerIP();
    void testGetServerIP();

};

void PFSUtilsTest::setUp()
{
}

void PFSUtilsTest::tearDown()
{
}

void PFSUtilsTest::testInstance()
{
    PFSUtils& utils = PFSUtils::instance();
    CPPUNIT_ASSERT(0 != &utils);
}

void PFSUtilsTest::testRegisterServerIP()
{
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
    // null ip
    IPvXAddress* ip0 = 0;

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
    CPPUNIT_ASSERT_EQUAL(ip0 , PFSUtils::instance().getServerIP(h7_1));
    CPPUNIT_ASSERT_EQUAL(ip0 , PFSUtils::instance().getServerIP(h7_1));
    CPPUNIT_ASSERT_EQUAL(ip0 , PFSUtils::instance().getServerIP(h7_2));
    CPPUNIT_ASSERT_EQUAL(ip0 , PFSUtils::instance().getServerIP(h7_3));
    CPPUNIT_ASSERT_EQUAL(ip7 , PFSUtils::instance().getServerIP(h7_4));

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
