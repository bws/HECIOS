#ifndef CONTIGUOUS_DATA_TYPE_TEST_H
#define CONTIGUOUS_DATA_TYPE_TEST_H
//
// This file is part of Hecios
//
// Copyright (C) 2007,2008,2009 Brad Settlemyer
//
// This file is distributed WITHOUT ANY WARRANTY. See the file 'License.txt'
// for details on this and other legal matters.
//
#include <iostream>
#include <string>
#include <cppunit/TestAssert.h>
#include <cppunit/extensions/HelperMacros.h>
#include "basic_data_type.h"
#include "contiguous_data_type.h"
using namespace std;

/** Unit test for ContiguousDataType */
class ContiguousDataTypeTest : public CppUnit::TestFixture
{
    // Create generic unit test and register test functions for automatic
    // exercise
    CPPUNIT_TEST_SUITE(ContiguousDataTypeTest);
    CPPUNIT_TEST(testConstructor);
    CPPUNIT_TEST(testGetRepresentationByteLength);
    CPPUNIT_TEST(testGetRegionsByBytes);
    CPPUNIT_TEST(testGetRegionsByCount);
    CPPUNIT_TEST_SUITE_END();

public:

    /** Called before each test function */
    void setUp();

    /** Called after each test function */
    void tearDown();

    void testConstructor();

    void testGetRepresentationByteLength();

    void testGetRegionsByBytes();

    void testGetRegionsByCount();

private:
};

void ContiguousDataTypeTest::setUp()
{
}

void ContiguousDataTypeTest::tearDown()
{
}

void ContiguousDataTypeTest::testConstructor()
{
    ByteDataType dt1;
    ContiguousDataType dt2(4, dt1);
}

void ContiguousDataTypeTest::testGetRepresentationByteLength()
{
    ByteDataType bdt1;
    ContiguousDataType cdt1(1, bdt1);
    CPPUNIT_ASSERT_EQUAL((size_t)4 + bdt1.getRepresentationByteLength(),
                         cdt1.getRepresentationByteLength());
}

void ContiguousDataTypeTest::testGetRegionsByBytes()
{
    DoubleDataType bdt1;
    ContiguousDataType cdt1(7, bdt1);
    vector<FileRegion> regions = cdt1.getRegionsByBytes(3, 8);
    CPPUNIT_ASSERT_EQUAL((size_t)1, regions.size());

    FileRegion fr1 = regions[0];
    CPPUNIT_ASSERT_EQUAL((FSOffset)3, fr1.offset);
    CPPUNIT_ASSERT_EQUAL((FSSize) DoubleDataType::MPI_DOUBLE_WIDTH,
                         fr1.extent);
}

void ContiguousDataTypeTest::testGetRegionsByCount()
{
    /*
    DoubleDataType bdt1;
    ContiguousDataType cdt1(7, bdt1);
    vector<FileRegion> regions = cdt1.getRegionsByCount(4, 4);
    CPPUNIT_ASSERT_EQUAL((size_t)1, regions.size());

    FileRegion fr1 = regions[0];
    CPPUNIT_ASSERT_EQUAL((FSOffset)4, fr1.offset);
    CPPUNIT_ASSERT_EQUAL((FSSize)4 * 7 * DoubleDataType::MPI_DOUBLE_WIDTH,
                         fr1.extent);
    */
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
