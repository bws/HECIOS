#ifndef UMD_IO_TRACE_TEST_H
#define UMD_IO_TRACE_TEST_H

#include <string>
#include <cppunit/extensions/HelperMacros.h>
#include <omnetpp.h>
#include "pfs_utils.h"
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
    CPPUNIT_TEST(testHasMoreRecords);
    CPPUNIT_TEST(testNextRecord);
    CPPUNIT_TEST_SUITE_END();

public:

    /** Called before each test function */
    void setUp();

    /** Called after each test function */
    void tearDown();
    
    void testConstructor();

    void testIsValid();

    void testHasMoreRecords();
    
    void testNextRecord();

    void testNextRecordAsMessage();
};

void UMDIOTraceTest::setUp()
{
    // Register servers for use during testing
    HandleRange range1 = {100, 200};
    HandleRange range2 = {2000, 3000};
    PFSUtils::instance().registerFSServer(range1, true);
    PFSUtils::instance().registerFSServer(range2, false);
}

void UMDIOTraceTest::tearDown()
{
    // Clear the singleton state between tests
    PFSUtils::clearState();
}

void UMDIOTraceTest::testConstructor()
{
    // Test with an invalid file
    UMDIOTrace test1(1, "dummy_file_name");
    CPPUNIT_ASSERT(false == test1.isValid());

    //Test with a valid trace file
    UMDIOTrace test2(1, "tests/support/umd_io_trace.tra");
    CPPUNIT_ASSERT(true == test2.isValid());
}

void UMDIOTraceTest::testIsValid()
{
    // Test with an invalid file
    UMDIOTrace test1(1, "dummy_file_name");
    CPPUNIT_ASSERT(false == test1.isValid());

    //Test with a valid trace file
    UMDIOTrace test2(1, "tests/support/umd_io_trace.tra");
    CPPUNIT_ASSERT(true == test2.isValid());
}

void UMDIOTraceTest::testHasMoreRecords()
{
    // Test with a valid trace
    UMDIOTrace test(1, "tests/support/umd_io_trace.tra");
    for (int i = 0; i < 10; i++)
    {
        CPPUNIT_ASSERT(true == test.hasMoreRecords());
        IOTraceRecord* rec = test.nextRecord();
        delete rec;
    }

    CPPUNIT_ASSERT(false == test.hasMoreRecords());
}

void UMDIOTraceTest::testNextRecord()
{
    UMDIOTrace test1(1, "tests/support/umd_io_trace.tra");

    //
    // 1st record
    //
    IOTraceRecord* rec1 = test1.nextRecord();
    CPPUNIT_ASSERT(0 != rec1);
    CPPUNIT_ASSERT_EQUAL(IOTrace::OPEN, rec1->opType());
    delete rec1;

    //
    // 2nd record
    //
    IOTraceRecord* rec2 = test1.nextRecord();
    CPPUNIT_ASSERT(0 != rec2);
    CPPUNIT_ASSERT_EQUAL(IOTrace::CLOSE, rec2->opType());
    delete rec2;

    //
    // 3rd record
    //
    IOTraceRecord* rec3 = test1.nextRecord();
    CPPUNIT_ASSERT(0 != rec3);
    CPPUNIT_ASSERT_EQUAL(IOTrace::READ_AT, rec3->opType());
    delete rec3;

    //
    // 4th record
    //
    IOTraceRecord* rec4 = test1.nextRecord();
    CPPUNIT_ASSERT(0 != rec4);
    CPPUNIT_ASSERT_EQUAL(IOTrace::WRITE_AT, rec4->opType());
    delete rec4;

    //
    // 5th record
    //
    IOTraceRecord* rec5 = test1.nextRecord();
    CPPUNIT_ASSERT(0 != rec5);
    CPPUNIT_ASSERT_EQUAL(IOTrace::SEEK, rec5->opType());
    delete rec5;

    //
    // 6th record
    //
    IOTraceRecord* rec6 = test1.nextRecord();
    CPPUNIT_ASSERT(0 == rec6);
    //CPPUNIT_ASSERT_EQUAL(IOTrace::OPEN, rec6->opType());
    delete rec6;

    //
    // 7th record
    //
    IOTraceRecord* rec7 = test1.nextRecord();
    CPPUNIT_ASSERT(0 != rec7);
    CPPUNIT_ASSERT_EQUAL(IOTrace::OPEN, rec7->opType());
    delete rec7;

    //
    // 8th record
    //
    IOTraceRecord* rec8 = test1.nextRecord();
    CPPUNIT_ASSERT(0 != rec8);
    CPPUNIT_ASSERT_EQUAL(IOTrace::CLOSE, rec8->opType());
    delete rec8;

    //
    // 9th record
    //
    IOTraceRecord* rec9 = test1.nextRecord();
    CPPUNIT_ASSERT(0 != rec9);
    CPPUNIT_ASSERT_EQUAL(IOTrace::READ_AT, rec9->opType());
    delete rec9;

    //
    // 10th record
    //
    IOTraceRecord* rec10 = test1.nextRecord();
    CPPUNIT_ASSERT(0 != rec10);
    CPPUNIT_ASSERT_EQUAL(IOTrace::WRITE_AT, rec10->opType());
    delete rec10;

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
