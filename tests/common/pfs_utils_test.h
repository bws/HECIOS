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
    CPPUNIT_TEST(testInstance);
    CPPUNIT_TEST_SUITE_END();

public:

    /** Called before each test function */
    void setUp();

    /** Called after each test function */
    void tearDown();
    
    void testInstance();

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

#endif

/*
 * Local variables:
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 *
 * vim: ts=8 sts=4 sw=4 expandtab
 */
