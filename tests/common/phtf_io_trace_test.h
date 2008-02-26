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
  CPPUNIT_TEST_SUITE_END();

public:
  void setUp();
  void tearDown() {};

  void testRecordConstructor1();
  void testRecordConstructor2();

  void testEventConstructor();
  void testEventRead();

  void testTraceConstructor();
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
  CPPUNIT_ASSERT_EQUAL(re.paraNum(), (long)2);
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
  PHTFEvent ev("/opt/public/lanl-traces/32PE_N-N_1024K/32PE_N-N_1024K.anon/phtf/event.0");
  CPPUNIT_ASSERT_EQUAL(ev.filePath(), string("/opt/public/lanl-traces/32PE_N-N_1024K/32PE_N-N_1024K.anon/phtf/event.0"));
}

void PHTFIOTraceTest::testEventRead()
{
  PHTFEvent ev("/opt/public/lanl-traces/32PE_N-N_1024K/32PE_N-N_1024K.anon/phtf/event.0");
  PHTFEventRecord re;
  CPPUNIT_ASSERT_EQUAL(ev.open(), (int)0);
  ev >> re;
  CPPUNIT_ASSERT_EQUAL(re.recordStr(), string("1 MPI_BARRIER 0 0.061279 0 91 0 512 0xb7f894a8 0x6f0fb4"));
  ev >> re >> re >> re >> re >> re >> re;
  CPPUNIT_ASSERT_EQUAL(re.recordStr(), string("7 MPI_FILE_OPEN 0.227128 0.146685 0 92 0x8077f98 37 0x8077cd8 0xbfe88b60"));
  while(!ev.eof())
    {
      ev >> re;
    };
  CPPUNIT_ASSERT_EQUAL(re.recordId(), (long)32771);
  ev.close();
}

void PHTFIOTraceTest::testTraceConstructor()
{
  PHTFTrace &tr = *PHTFTrace::getInstance("/opt/public/lanl-traces/32PE_N-N_1024K/32PE_N-N_1024K.anon/phtf/");
  PHTFEvent &ev = *(tr.getEvent(0));
  CPPUNIT_ASSERT_EQUAL(ev.filePath(), string("/opt/public/lanl-traces/32PE_N-N_1024K/32PE_N-N_1024K.anon/phtf/event.0"));
  PHTFEventRecord re;
  ev.open();
  ev >> re;
  CPPUNIT_ASSERT_EQUAL(re.recordStr(), string("1 MPI_BARRIER 0 0.061279 0 91 0 512 0xb7f894a8 0x6f0fb4"));
  ev.close();
}
