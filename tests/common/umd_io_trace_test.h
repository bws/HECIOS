#ifndef UMD_IO_TRACE_TEST_H
#define UMD_IO_TRACE_TEST_H

#include <string>
#include <cppunit/extensions/HelperMacros.h>
#include <omnetpp.h>
#include "mpiio_proto_m.h"
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
    CPPUNIT_TEST(testNextRecordAsMessage);
    CPPUNIT_TEST_SUITE_END();

public:

    /** Called before each test function */
    void setUp() {};

    /** Called after each test function */
    void tearDown() {};
    
    void testConstructor();

    void testIsValid();

    void testHasMoreRecords();
    
    void testNextRecord();

    void testNextRecordAsMessage();
};

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
        cMessage* msg = test.nextRecordAsMessage();
        delete msg;
    }

    CPPUNIT_ASSERT(false == test.hasMoreRecords());
}

void UMDIOTraceTest::testNextRecord()
{
}

void UMDIOTraceTest::testNextRecordAsMessage()
{
    UMDIOTrace test1(1, "tests/support/umd_io_trace.tra");
    cMessage* msg;
    mpiFileOpenRequest* open;
    mpiFileCloseRequest* close;
    mpiFileReadAtRequest* read;
    mpiFileWriteAtRequest* write;

    //
    // 1st record
    //
    msg = test1.nextRecordAsMessage();
    CPPUNIT_ASSERT(0 != msg);
    open = dynamic_cast<mpiFileOpenRequest*>(msg);
    CPPUNIT_ASSERT(0 != open);
    delete open;

    //
    // 2nd record
    //
    msg = test1.nextRecordAsMessage();
    CPPUNIT_ASSERT(0 != msg);
    close = dynamic_cast<mpiFileCloseRequest*>(msg);
    CPPUNIT_ASSERT(0 != close);
    delete close;

    //
    // 3rd record
    //
    msg = test1.nextRecordAsMessage();
    CPPUNIT_ASSERT(0 != msg);
    read = dynamic_cast<mpiFileReadAtRequest*>(msg);
    CPPUNIT_ASSERT(0 != read);
    delete read;

    //
    // 4th record
    //
    msg = test1.nextRecordAsMessage();
    CPPUNIT_ASSERT(0 != msg);
    write = dynamic_cast<mpiFileWriteAtRequest*>(msg);
    CPPUNIT_ASSERT(0 != write);
    delete write;

    //
    // 5th record -- seek
    //
    msg = test1.nextRecordAsMessage();
    CPPUNIT_ASSERT(0 != msg);
    read = dynamic_cast<mpiFileReadAtRequest*>(msg);
    CPPUNIT_ASSERT(0 != read);
    delete read;

    //
    // 6th record (at present listio_header messages return null)
    //
    msg = test1.nextRecordAsMessage();
    CPPUNIT_ASSERT(0 == msg);
    delete msg;

    //
    // 7th record
    //
    msg = test1.nextRecordAsMessage();
    CPPUNIT_ASSERT(0 != msg);
    open = dynamic_cast<mpiFileOpenRequest*>(msg);
    CPPUNIT_ASSERT(0 != open);
    delete open;

    //
    // 8th record
    //
    msg = test1.nextRecordAsMessage();
    CPPUNIT_ASSERT(0 != msg);
    close = dynamic_cast<mpiFileCloseRequest*>(msg);
    CPPUNIT_ASSERT(0 != close);
    delete close;

    //
    // 9th record
    //
    msg = test1.nextRecordAsMessage();
    CPPUNIT_ASSERT(0 != msg);
    read = dynamic_cast<mpiFileReadAtRequest*>(msg);
    CPPUNIT_ASSERT(0 != read);
    delete read;

    //
    // 10th record
    //
    msg = test1.nextRecordAsMessage();
    CPPUNIT_ASSERT(0 != msg);
    write = dynamic_cast<mpiFileWriteAtRequest*>(msg);
    CPPUNIT_ASSERT(0 != write);
    delete write;

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
