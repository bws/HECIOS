#include <cppunit/extensions/HelperMacros.h>
#include "filename.h"
using namespace std;

/** Unit test for Filename */
class FilenameTest : public CppUnit::TestFixture
{
    // Create generic unit test and register test functions for automatic
    // exercise
    CPPUNIT_TEST_SUITE(FilenameTest);
    CPPUNIT_TEST(testStr);
    CPPUNIT_TEST(testGetNumPathSegments);
    CPPUNIT_TEST(testGetSegment);
    CPPUNIT_TEST_SUITE_END();

public:

    /** Called before each test function */
    void setUp() {};

    /** Called after each test function */
    void tearDown() {};

    void testStr();
    void testGetNumPathSegments();
    void testGetSegment();
};

void FilenameTest::testStr()
{
    Filename f1("/");
    CPPUNIT_ASSERT_EQUAL(string("/"), f1.str());

    Filename f2("/foo");
    CPPUNIT_ASSERT_EQUAL(string("/foo"), f2.str());

    Filename f3("//bar");
    CPPUNIT_ASSERT_EQUAL(string("/bar"), f3.str());

    Filename f4("//baz//");
    CPPUNIT_ASSERT_EQUAL(string("/baz"), f4.str());
}

void FilenameTest::testGetNumPathSegments()
{
    Filename f1("/");
    CPPUNIT_ASSERT_EQUAL(1u, f1.getNumPathSegments());

    Filename f2("/foo");
    CPPUNIT_ASSERT_EQUAL(2u, f2.getNumPathSegments());

    Filename f3("/foo/bar");
    CPPUNIT_ASSERT_EQUAL(3u, f3.getNumPathSegments());

    Filename f4("/foo/bar/baz/");
    CPPUNIT_ASSERT_EQUAL(4u, f4.getNumPathSegments());

    Filename f5("/foo/////");
    CPPUNIT_ASSERT_EQUAL(2u, f5.getNumPathSegments());

    Filename f6("/foo/////bar");
    CPPUNIT_ASSERT_EQUAL(3u, f6.getNumPathSegments());
}

void FilenameTest::testGetSegment()
{
    Filename f1("/");
    CPPUNIT_ASSERT_EQUAL(Filename("/"), f1.getSegment(0));

    Filename f2("/bar");
    CPPUNIT_ASSERT_EQUAL(Filename("/"), f2.getSegment(0));
    CPPUNIT_ASSERT_EQUAL(Filename("/bar"), f2.getSegment(1));

    Filename f3("//foo/baz//");
    CPPUNIT_ASSERT_EQUAL(Filename("/"), f3.getSegment(0));
    CPPUNIT_ASSERT_EQUAL(Filename("/foo"), f3.getSegment(1));
    CPPUNIT_ASSERT_EQUAL(Filename("/foo/baz"), f3.getSegment(2));
}

/*
 * Local variables:
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 *
 * vim: ts=8 sts=4 sw=4 expandtab
 */
