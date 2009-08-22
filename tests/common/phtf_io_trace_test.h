#ifndef PHTF_IO_TRACE_TEST_H
#define PHTF_IO_TRACE_TEST_H
//
// This file is part of Hecios
//
// Copyright (C) 2007,2008,2009 Brad Settlemyer
//
// This file is distributed WITHOUT ANY WARRANTY. See the file 'License.txt'
// for details on this and other legal matters.
//
#include <string>
#include <vector>
#include <cppunit/extensions/HelperMacros.h>
#include "phtf_io_trace.h"


class PHTFIOTraceTest : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(PHTFIOTraceTest);
    CPPUNIT_TEST(testRecordConstructor1);
    CPPUNIT_TEST(testRecordConstructor2);
    CPPUNIT_TEST(testEventConstructor);
    CPPUNIT_TEST(testEventRead);
    CPPUNIT_TEST(testTraceConstructor);
    CPPUNIT_TEST(testIniHandler);
    CPPUNIT_TEST_SUITE_END();

public:
    void setUp();
    void tearDown() {};

    void testRecordConstructor1();
    void testRecordConstructor2();

    void testEventConstructor();
    void testEventRead();

    void testTraceConstructor();

    void testIniHandler();
};

void PHTFIOTraceTest::setUp()
{
    PHTFEventRecord::buildOpMap();
}

void PHTFIOTraceTest::testRecordConstructor1()
{
    PHTFEventRecord re("1 MPI_FILE_WRITE 2.2301 0.0031 4 123 234");
    CPPUNIT_ASSERT_EQUAL(re.recordStr(), string("1 MPI_FILE_WRITE 2.2301 0.0031 4 123 234"));
    CPPUNIT_ASSERT_EQUAL(re.recordId(), (long)1);
    CPPUNIT_ASSERT_EQUAL(re.recordOp(), WRITE);
    CPPUNIT_ASSERT_EQUAL(re.startTime(), (double)2.2301);
    CPPUNIT_ASSERT_EQUAL(re.duration(), (double)0.0031);
    CPPUNIT_ASSERT_EQUAL(re.retValue(), (long)4);
    CPPUNIT_ASSERT_EQUAL(re.params(), string("123 234"));
    CPPUNIT_ASSERT_EQUAL(re.paraNum(), size_t(2));
    CPPUNIT_ASSERT_EQUAL(re.paramAt(0), string("123"));
    CPPUNIT_ASSERT_EQUAL(re.paramAt(1), string("234"));
}

void PHTFIOTraceTest::testRecordConstructor2()
{
    std::vector <std::string> paras;
    paras.push_back("123");
    paras.push_back("234");
    PHTFEventRecord re(1, WRITE, 2.2301, 0.0031, 4, paras);
    CPPUNIT_ASSERT_EQUAL(re.recordStr(), string("1 MPI_FILE_WRITE 2.2301 0.0031 4 123 234"));
}

void PHTFIOTraceTest::testEventConstructor()
{
    PHTFEvent event("tests/traces/phtf/event.0");
    CPPUNIT_ASSERT_EQUAL(event.filePath(), string("tests/traces/phtf/event.0"));
}

void PHTFIOTraceTest::testEventRead()
{
    PHTFEvent event("tests/traces/phtf/event.0");
    PHTFEventRecord re;
    CPPUNIT_ASSERT_EQUAL(event.open(), (int)0);
    event >> re;
    CPPUNIT_ASSERT_EQUAL(re.recordStr(), string("1 MPI_BARRIER 0 0.061279 0 91 0 512 0xb7f894a8 0x6f0fb4"));
    event >> re >> re >> re >> re >> re >> re;
    CPPUNIT_ASSERT_EQUAL(re.recordStr(), string("7 MPI_FILE_OPEN 0.227128 0.146685 0 92 0x8077f98 37 0x8077cd8 0xbfe88b60"));
    while(!event.eof())
    {
        event >> re;
    };
    CPPUNIT_ASSERT_EQUAL(re.recordId(), (long)19);
    event.close();
}

void PHTFIOTraceTest::testTraceConstructor()
{
    PHTFTrace& tr = PHTFTrace::instance();
    tr.dirPath("tests/traces/phtf/");
    PHTFEvent& event = *(tr.getEvent(0));
    CPPUNIT_ASSERT_EQUAL(event.filePath(), string("tests/traces/phtf/event.0"));
    PHTFEventRecord re;
    event.open();
    event >> re;
    CPPUNIT_ASSERT_EQUAL(re.recordStr(), string("1 MPI_BARRIER 0 0.061279 0 91 0 512 0xb7f894a8 0x6f0fb4"));
    event.close();
}

void PHTFIOTraceTest::testIniHandler()
{
    PHTFIni ini("tests/traces/phtf/test.ini");

    ini.iniValue("Sec1", "F1", "V1");
    ini.iniValue("Sec1", "F2", "V2");
    ini.iniValue("Sec2", "F1", "V1");
    ini.iniValue("Sec2", "F2", "V2");
    ini.iniValue("Sec1", "F3", "V3");
    ini.iniValue("Sec2", "F3", "V3");

    CPPUNIT_ASSERT_EQUAL(ini.iniValue("Sec1", "F1"), string("V1"));
    CPPUNIT_ASSERT_EQUAL(ini.iniValue("Sec1", "F2"), string("V2"));
    CPPUNIT_ASSERT_EQUAL(ini.iniValue("Sec1", "F3"), string("V3"));
    CPPUNIT_ASSERT_EQUAL(ini.iniValue("Sec2", "F2"), string("V2"));
    CPPUNIT_ASSERT_EQUAL(ini.iniValue("Sec2", "F1"), string("V1"));
    CPPUNIT_ASSERT_EQUAL(ini.iniValue("Sec2", "F3"), string("V3"));
}

#endif
