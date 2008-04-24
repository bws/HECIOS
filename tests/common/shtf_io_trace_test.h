#ifndef SHTF_IO_TRACE_TEST_H
#define SHTF_IO_TRACE_TEST_H
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
#include <string>
#include <cppunit/extensions/HelperMacros.h>
#include <omnetpp.h>
#include "file_builder.h"
#include "pfs_utils.h"
#include "shtf_io_trace.h"
using namespace std;

/** Unit test for SHTFTrace */
class SHTFIOTraceTest : public CppUnit::TestFixture
{
    // Create generic unit test and register test functions for automatic
    // exercise
    CPPUNIT_TEST_SUITE(SHTFIOTraceTest);
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

void SHTFIOTraceTest::setUp()
{
    // Register servers for use during testing
    HandleRange range1 = {100, 200};
    HandleRange range2 = {2000, 3000};
    FileBuilder::instance().registerFSServer(range1, true);
    FileBuilder::instance().registerFSServer(range2, false);
}

void SHTFIOTraceTest::tearDown()
{
    // Clear the singleton state between tests
    PFSUtils::clearState();
}

void SHTFIOTraceTest::testConstructor()
{
    // Test with an invalid file
    try {
        SHTFIOTrace test1("shtf_dummy_name");
        CPPUNIT_FAIL("Should not be able to open non-existant file.");
    }
    catch(...) {}

    //Test with a valid trace file
    SHTFIOTrace test2("tests/traces/test_trace.shtf");
    CPPUNIT_ASSERT(true == test2.isValid());
}

void SHTFIOTraceTest::testIsValid()
{
    //Test with a valid trace file
    SHTFIOTrace test2("tests/traces/test_trace.shtf");
    CPPUNIT_ASSERT(true == test2.isValid());
}

void SHTFIOTraceTest::testHasMoreRecords()
{
    // Test with a valid trace
    SHTFIOTrace test("tests/traces/test_trace.shtf");
    for (size_t i = 0; i < test.getNumRecords(); i++)
    {
        CPPUNIT_ASSERT(true == test.hasMoreRecords());
        IOTrace::Record* rec = test.nextRecord();
        //cerr << "i: " << i << " num recs: " << test.getNumRecords() << endl;
        delete rec;
    }

    CPPUNIT_ASSERT(false == test.hasMoreRecords());
}

void SHTFIOTraceTest::testNextRecord()
{
    SHTFIOTrace test1("tests/traces/test_trace.shtf");

    //
    // 1st record
    //
    IOTrace::Record* rec1 = test1.nextRecord();
    CPPUNIT_ASSERT(0 != rec1);
    CPPUNIT_ASSERT_EQUAL(IOTrace::MKDIR, rec1->opType());
    delete rec1;

    //
    // 2nd record
    //
    IOTrace::Record* rec2 = test1.nextRecord();
    CPPUNIT_ASSERT(0 != rec2);
    CPPUNIT_ASSERT_EQUAL(IOTrace::RMDIR, rec2->opType());
    delete rec2;

    //
    // 3rd record
    //
    IOTrace::Record* rec3 = test1.nextRecord();
    CPPUNIT_ASSERT(0 != rec3);
    CPPUNIT_ASSERT_EQUAL(IOTrace::STAT, rec3->opType());
    delete rec3;

    //
    // 4th record
    //
    IOTrace::Record* rec4 = test1.nextRecord();
    CPPUNIT_ASSERT(0 != rec4);
    CPPUNIT_ASSERT_EQUAL(IOTrace::UTIME, rec4->opType());
    delete rec4;

    //
    // 6th record
    //
    IOTrace::Record* rec6 = test1.nextRecord();
    CPPUNIT_ASSERT(0 != rec6);
    CPPUNIT_ASSERT_EQUAL(IOTrace::OPEN, rec6->opType());
    delete rec6;

    //
    // 7th record
    //
    IOTrace::Record* rec7 = test1.nextRecord();
    CPPUNIT_ASSERT(0 != rec7);
    CPPUNIT_ASSERT_EQUAL(IOTrace::READDIR, rec7->opType());
    delete rec7;
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
