#ifndef BASIC_DATA_TYPE_TEST_H
#define BASIC_DATA_TYPE_TEST_H
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
using namespace std;

/** Unit test for BasicDataType */
class BasicDataTypeTest : public CppUnit::TestFixture
{
    // Create generic unit test and register test functions for automatic
    // exercise
    CPPUNIT_TEST_SUITE(BasicDataTypeTest);
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

void BasicDataTypeTest::setUp()
{
}

void BasicDataTypeTest::tearDown()
{
}

void BasicDataTypeTest::testConstructor()
{
    BasicDataType<1> t;
}

void BasicDataTypeTest::testGetRepresentationByteLength()
{
    BasicDataType<1> dt1;
    CPPUNIT_ASSERT_EQUAL((size_t)4, dt1.getRepresentationByteLength());

    BasicDataType<2> dt2;
    CPPUNIT_ASSERT_EQUAL((size_t)4, dt2.getRepresentationByteLength());

    BasicDataType<16> dt3;
    CPPUNIT_ASSERT_EQUAL((size_t)4, dt3.getRepresentationByteLength());
}

void BasicDataTypeTest::testGetRegionsByBytes()
{
    DoubleDataType dt1;
    vector<FileRegion> regions = dt1.getRegionsByBytes(3, 8);
    CPPUNIT_ASSERT_EQUAL((size_t)1, regions.size());
    FileRegion fr1 = regions[0];
    CPPUNIT_ASSERT_EQUAL((FSOffset)3, fr1.offset);
    CPPUNIT_ASSERT_EQUAL((FSSize)1 * DoubleDataType::MPI_DOUBLE_WIDTH,
                         fr1.extent);
}

void BasicDataTypeTest::testGetRegionsByCount()
{
    DoubleDataType dt1;
    vector<FileRegion> regions = dt1.getRegionsByCount(4, 4);
    CPPUNIT_ASSERT_EQUAL((size_t)1, regions.size());
    FileRegion fr1 = regions[0];
    CPPUNIT_ASSERT_EQUAL((FSOffset)4, fr1.offset);
    CPPUNIT_ASSERT_EQUAL((FSSize)4 * DoubleDataType::MPI_DOUBLE_WIDTH,
                         fr1.extent);
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
