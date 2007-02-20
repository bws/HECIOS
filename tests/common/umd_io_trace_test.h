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
    mpiFileReadRequest* read;
    mpiFileWriteRequest* write;

    //
    // 1st record
    //
    msg = test1.nextRecordAsMessage();
    CPPUNIT_ASSERT(0 != msg);

    // Cast message and check it
    open = dynamic_cast<mpiFileOpenRequest*>(msg);
    CPPUNIT_ASSERT(0 != open);
    CPPUNIT_ASSERT(1);

    //
    // 2nd record
    //
    msg = test1.nextRecordAsMessage();
    CPPUNIT_ASSERT(0 != msg);

    // Cast message and check it
    close = dynamic_cast<mpiFileCloseRequest*>(msg);
    CPPUNIT_ASSERT(0 != read);

    //
    // 3rd record
    //
    msg = test1.nextRecordAsMessage();
    CPPUNIT_ASSERT(0 != msg);

    // Cast message and check it
    read = dynamic_cast<mpiFileReadRequest*>(msg);
    CPPUNIT_ASSERT(0 != read);

    //
    // 4th record
    //
    msg = test1.nextRecordAsMessage();
    CPPUNIT_ASSERT(0 != msg);

    // Cast message and check it
    write = dynamic_cast<mpiFileWriteRequest*>(msg);
    CPPUNIT_ASSERT(0 != read);

    //
    // 5th record
    //
    msg = test1.nextRecordAsMessage();
    CPPUNIT_ASSERT(0 != msg);

    // Cast message and check it
    //seek = dynamic_cast<mpiFileReadRequest*>(msg);
    //CPPUNIT_ASSERT(0 != read);

    //
    // 6th record
    //
    msg = test1.nextRecordAsMessage();
    CPPUNIT_ASSERT(0 != msg);

    //
    // 7th record
    //
    msg = test1.nextRecordAsMessage();
    CPPUNIT_ASSERT(0 != msg);

    //
    // 8th record
    //
    msg = test1.nextRecordAsMessage();
    CPPUNIT_ASSERT(0 != msg);

    //
    // 9th record
    //
    msg = test1.nextRecordAsMessage();
    CPPUNIT_ASSERT(0 != msg);

    //
    // 10th record
    //
    msg = test1.nextRecordAsMessage();
    CPPUNIT_ASSERT(0 != msg);

    
    
}

#endif
