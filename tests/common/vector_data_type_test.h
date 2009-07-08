#ifndef VECTOR_DATA_TYPE_TEST_H
#define VECTOR_DATA_TYPE_TEST_H
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
#include "vector_data_type.h"
using namespace std;

/** Unit test for VectorDataType */
class VectorDataTypeTest : public CppUnit::TestFixture
{
    // Create generic unit test and register test functions for automatic
    // exercise
    CPPUNIT_TEST_SUITE(VectorDataTypeTest);
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

void VectorDataTypeTest::setUp()
{
}

void VectorDataTypeTest::tearDown()
{
}

void VectorDataTypeTest::testConstructor()
{
    ByteDataType dt1;
    VectorDataType dt2(3, 2, 3, dt1);
    VectorDataType dt3(3, 2, 1, dt1);
    VectorDataType dt4(3, 2, -3, dt1);
}

void VectorDataTypeTest::testGetRepresentationByteLength()
{
    ByteDataType bdt1;
    VectorDataType vdt1(3, 2, 3, bdt1);
    CPPUNIT_ASSERT_EQUAL((size_t)12 + bdt1.getRepresentationByteLength(),
                         vdt1.getRepresentationByteLength());
}

void VectorDataTypeTest::testGetRegionsByBytes()
{
    // Test a simple scenario
    DoubleDataType bdt1;
    VectorDataType vdt1(3,2,3, bdt1);


    // Get the regions from the beginning
    vector<FileRegion> regions =
        vdt1.getRegionsByBytes(0, DoubleDataType::MPI_DOUBLE_WIDTH * 10);

    // Verify that 5 regions are returned
    CPPUNIT_ASSERT_EQUAL((size_t)5, regions.size());

    // Verify that the 1st region begins at 0 and extends for 2
    FileRegion fr0 = regions[0];
    CPPUNIT_ASSERT_EQUAL((FSOffset)0, fr0.offset);
    CPPUNIT_ASSERT_EQUAL((FSSize)2 * DoubleDataType::MPI_DOUBLE_WIDTH,
                         fr0.extent);

    // Verify that the 2nd region begins at 24 and extends for 2
    FileRegion fr1 = regions[1];
    CPPUNIT_ASSERT_EQUAL((FSOffset)(3 * DoubleDataType::MPI_DOUBLE_WIDTH),
                         fr1.offset);
    CPPUNIT_ASSERT_EQUAL((FSSize)2 * DoubleDataType::MPI_DOUBLE_WIDTH,
                         fr1.extent);

    // Verify that the 2nd region begins at 48 and extends for 2
    FileRegion fr2 = regions[2];
    CPPUNIT_ASSERT_EQUAL((FSOffset)(6 * DoubleDataType::MPI_DOUBLE_WIDTH),
                         fr2.offset);
    CPPUNIT_ASSERT_EQUAL((FSSize)2 * DoubleDataType::MPI_DOUBLE_WIDTH,
                         fr2.extent);

    //
    // Test 2
    //
    // Get the regions from the center of the data type
    regions = vdt1.getRegionsByBytes(2, 40);

    // Verify that 3 regions are returned
    CPPUNIT_ASSERT_EQUAL((size_t)3, regions.size());

    // Verify they first regions begins at 2, and extends for 14 bytes
    CPPUNIT_ASSERT_EQUAL(FSOffset(2), regions[0].offset);
    CPPUNIT_ASSERT_EQUAL(FSSize(14), regions[0].extent);

    // Verify they first regions begins at 2, and extends for 14 bytes
    CPPUNIT_ASSERT_EQUAL(FSOffset(24), regions[1].offset);
    CPPUNIT_ASSERT_EQUAL(FSSize(16), regions[1].extent);

    // Verify they first regions begins at 2, and extends for 14 bytes
    CPPUNIT_ASSERT_EQUAL(FSOffset(48), regions[2].offset);
    CPPUNIT_ASSERT_EQUAL(FSSize(10), regions[2].extent);
}

void VectorDataTypeTest::testGetRegionsByCount()
{
    /*
    // Test a simple scenario
    DoubleDataType bdt1;
    VectorDataType vdt1(3,2,3, bdt1);
    vector<FileRegion> regions = vdt1.getRegionsByCount(0, 1);

    for (size_t i = 0; i < regions.size(); i++)
    {
        //cerr << regions[i].offset << " " << regions[i].extent << endl;
    }

    // Verify the correct number of regions are returned
    CPPUNIT_ASSERT_EQUAL((size_t)3, regions.size());

    // Verify 1st region begins at 0 and extends for 2
    FileRegion fr0 = regions[0];
    CPPUNIT_ASSERT_EQUAL((FSOffset)0, fr0.offset);
    CPPUNIT_ASSERT_EQUAL((FSSize)2 * DoubleDataType::MPI_DOUBLE_WIDTH,
                         fr0.extent);

    // Verify 2nd region begins at 3 and extends for 2
    FileRegion fr1 = regions[1];
    CPPUNIT_ASSERT_EQUAL((FSOffset)(3 * DoubleDataType::MPI_DOUBLE_WIDTH),
                         fr1.offset);
    CPPUNIT_ASSERT_EQUAL((FSSize)2 * DoubleDataType::MPI_DOUBLE_WIDTH,
                         fr1.extent);

    // Verify 3rd region begins at 6 and extends for 2
    FileRegion fr2 = regions[2];
    CPPUNIT_ASSERT_EQUAL((FSOffset)(6 * DoubleDataType::MPI_DOUBLE_WIDTH),
                         fr2.offset);
    CPPUNIT_ASSERT_EQUAL((FSSize)2 * DoubleDataType::MPI_DOUBLE_WIDTH,
                         fr2.extent);
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
