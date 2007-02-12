
#include <string>
#include <cppunit/extensions/HelperMacros.h>
#include "umd_io_trace.h"
using namespace std;

/** Unit test for UMDIOTrace */
class UMDIOTraceTest : public CppUnit::TestFixture
{
    // Create generic unit test and register test functions for automatic
    // exercise
    CPPUNIT_TEST_SUITE(UMDIOTraceTest);
    CPPUNIT_TEST(testConstructor);
    CPPUNIT_TEST(testIsValid);
    CPPUNIT_TEST(testNextRecord);
    CPPUNIT_TEST(testNextRecordAsMessage);
    CPPUNIT_TEST_SUITE_END();

public:

    /** Called before each test function */
    void setUp() {};

    /** Called after each test function */
    void tearDown() {};
    
    void testConstructor() { CPPUNIT_ASSERT(1 == 1); };

    void testIsValid() { CPPUNIT_ASSERT(1 == 1); };

    void testNextRecord() { CPPUNIT_ASSERT(1 == 1); };

    void testNextRecordAsMessage() { CPPUNIT_ASSERT(1 == 1); };
};
